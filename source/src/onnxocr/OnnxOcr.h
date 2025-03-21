#pragma once
#include <string>
#include <windows.h>
#include <atlimage.h>
#include <memory>
#include <numeric>
#include <fstream>
#include <src/onnxruntime/include/onnxruntime_cxx_api.h>
#pragma comment(lib,"onnxruntime.lib")

class OnnxOcr
{
	std::unique_ptr<Ort::Env> m_env;
	std::unique_ptr<Ort::Session> m_session;
	char* m_inputName = nullptr;
	char* m_outputName = nullptr;
	std::vector<std::string> m_keys;
	size_t m_scaleSize = 48;
	bool m_init = false;
	static constexpr float s_meanValue = 127.5f;
	static constexpr float s_normValue = 1.0 / s_meanValue;
public:
	enum Result
	{
		r_ok,
		r_model_notfound,
		r_keys_notfound,
		r_model_invalid,
		r_keys_invalid,
		r_sdk_different
	};

	~OnnxOcr()
	{
		release();
	}

	void release()
	{
		m_keys.clear();
		m_init = false;
		if (m_inputName)
		{
			free(m_inputName);
			m_inputName = nullptr;
		}
		if (m_outputName)
		{
			free(m_outputName);
			m_outputName = nullptr;
		}
	}

	bool isInit() const { return m_init; }

	Result init(const std::string& model, const std::string& keys, size_t threads = 2, size_t scaleSize = 48)
	{
		release();
		if (!threads) threads = 1;
		m_scaleSize = scaleSize;

		std::ifstream modelFile(model);
		if (!modelFile) return r_model_notfound;

		std::ifstream keysFile(keys);
		if (!keysFile) return r_keys_notfound;

		std::string line;
		while (std::getline(keysFile, line)) m_keys.push_back(line);

		if (m_keys.empty()) return r_keys_invalid;

		if (!Ort::Global<void>::api_) return r_sdk_different;
		try
		{
			m_env = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_ERROR, "OnnxOcr");
		}
		catch (...)
		{
			return r_sdk_different;
		}
		try
		{
			Ort::SessionOptions options;
			options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
			options.SetInterOpNumThreads(threads);

			m_session = std::make_unique<Ort::Session>(*m_env, toWString(model).c_str(), options);

			size_t inputCount = m_session->GetInputCount();
			if (!inputCount) return r_model_invalid;

			Ort::AllocatorWithDefaultOptions allocator;
			Ort::AllocatedStringPtr s = m_session->GetInputNameAllocated(0, allocator);
			m_inputName = strdup(s.get());

			s = m_session->GetOutputNameAllocated(0, allocator);
			m_outputName = strdup(s.get());
		}
		catch (...)
		{
			return r_model_invalid;
		}

		m_init = true;
		return r_ok;
	}

	std::vector<float> makeTensorValues(const CImage& src) {
		int width = src.GetWidth();
		int height = src.GetHeight();
		int bpp = src.GetBPP();
		int numChannels = (bpp == 24) ? 3 : 4;
		size_t imageSize = width * height;

		std::vector<float> inputTensorValues(imageSize * 3);

		BYTE* pixelData = (BYTE*)src.GetBits();
		int pitch = src.GetPitch();

		for (int y = 0; y < height; ++y) {
			BYTE* row = pixelData + y * pitch;
			for (int x = 0; x < width; ++x) {
				BYTE blue = row[x * numChannels];
				BYTE green = row[x * numChannels + 1];
				BYTE red = row[x * numChannels + 2];

				float normRed = (static_cast<float>(red) - s_meanValue) * s_normValue;
				float normGreen = (static_cast<float>(green) - s_meanValue) * s_normValue;
				float normBlue = (static_cast<float>(blue) - s_meanValue) * s_normValue;

				size_t index = y * width + x;
				inputTensorValues[index] = normRed;
				inputTensorValues[imageSize + index] = normGreen;
				inputTensorValues[2 * imageSize + index] = normBlue;
			}
		}

		return inputTensorValues;
	}

	std::string scoreToString(const std::vector<float>& outputData, int h, int w)
	{
		std::string result;
		int indexPrev = 0;
		for (int i = 0; i < h; i++) {
			int firstChar = i * w;
			int lastChar = i * w + w;
			int index = std::distance(outputData.begin() + firstChar, std::max_element(outputData.begin() + firstChar, outputData.begin() + lastChar));
			if (index > 0 && index <= m_keys.size() && index != indexPrev) result += m_keys[index - 1];
			indexPrev = index;
		}
		return result;
	}

	std::string scan(const CImage& image)
	{
		if (!isInit()) return std::string();
		if (image.IsNull()) return std::string();
		if (image.GetBPP() < 24) return std::string();
		CImage imageScaled = resizeWithHeight(image, m_scaleSize);
		std::vector<float> tensorValues = makeTensorValues(imageScaled);
		std::vector<int64_t> inputShape{ 1, 3, imageScaled.GetHeight(), imageScaled.GetWidth() };
		try
		{
			Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);

			Ort::Value inputTensor = Ort::Value::CreateTensor<float>(memoryInfo, tensorValues.data(), tensorValues.size(), inputShape.data(), inputShape.size());
			if (!inputTensor.IsTensor()) return std::string();

			std::vector<Ort::Value> outputTensor = m_session->Run(Ort::RunOptions{}, &m_inputName, &inputTensor, 1, &m_outputName, 1);
			if (outputTensor.empty() || outputTensor.size() != 1 || !outputTensor.front().IsTensor()) return std::string();

			std::vector<int64_t> outputShape = outputTensor.front().GetTensorTypeAndShapeInfo().GetShape();

			int64_t outputCount = std::accumulate(outputShape.begin(), outputShape.end(), 1, std::multiplies<int64_t>());

			float* floatArray = outputTensor.front().GetTensorMutableData<float>();
			std::vector<float> outputData(floatArray, floatArray + outputCount);
			return scoreToString(outputData, outputShape[1], outputShape[2]);
		}
		catch (...)
		{
			return std::string();
		}
	}

	static CImage resizeWithHeight(const CImage& srcImage, size_t height) {
		int newWidth = (srcImage.GetWidth() * height) / srcImage.GetHeight();
		CImage destImage;
		destImage.Create(newWidth, height, srcImage.GetBPP());
		srcImage.StretchBlt(destImage.GetDC(), 0, 0, newWidth, height, SRCCOPY);
		destImage.ReleaseDC();
		return destImage;
	}

	static std::string toString(std::wstring val, UINT codePage = CP_UTF8)
	{
		int length = WideCharToMultiByte(codePage, 0, val.c_str(), val.size(), 0, 0, 0, 0);
		if (length)
		{
			std::string result(length, 0);
			length = WideCharToMultiByte(codePage, 0, val.c_str(), val.size(), &result[0], length, 0, 0);
			if (length) return result;
		}
		return std::string();
	}

	static std::wstring toWString(std::string val, DWORD codePage = CP_UTF8)
	{
		int length = MultiByteToWideChar(codePage, 0, val.c_str(), val.size(), 0, 0);
		if (length)
		{
			std::wstring result(length, 0);
			length = MultiByteToWideChar(codePage, 0, val.c_str(), val.size(), &result[0], length);
			if (length) return result;
		}
		return std::wstring();
	}
};

class QiOnnxOcr : private OnnxOcr
{
public:
	QiOnnxOcr()
	{
		SYSTEM_INFO info; GetSystemInfo(&info);
		size_t threads = info.dwNumberOfProcessors >> 1;
		if (threads < 2) threads = 2;
		OnnxOcr::Result result = OnnxOcr::init("OCR\\ppocr.onnx", "OCR\\ppocr.keys", threads, 48);
		switch (result)
		{
		case OnnxOcr::r_ok:
			break;
		case OnnxOcr::r_model_notfound:
			MessageBoxW(nullptr, L"模型不存在", L"OCR初始化错误", MB_ICONERROR);
			break;
		case OnnxOcr::r_keys_notfound:
			MessageBoxW(nullptr, L"Key不存在", L"OCR初始化错误", MB_ICONERROR);
			break;
		case OnnxOcr::r_model_invalid:
			MessageBoxW(nullptr, L"模型无效", L"OCR初始化错误", MB_ICONERROR);
			break;
		case OnnxOcr::r_keys_invalid:
			MessageBoxW(nullptr, L"Key无效", L"OCR初始化错误", MB_ICONERROR);
			break;
		case OnnxOcr::r_sdk_different:
			MessageBoxW(nullptr, L"Sdk不一致", L"OCR初始化错误", MB_ICONERROR);
			break;
		default:
			MessageBoxW(nullptr, L"未知错误", L"OCR初始化错误", MB_ICONERROR);
			break;
		}
	}
	bool isInit() const { return OnnxOcr::isInit(); }
	std::string scan(const CImage& image)
	{
		return OnnxOcr::scan(image);
	}
	std::string scan(const RECT& rect)
	{
		int w = rect.right - rect.left;
		int h = rect.bottom - rect.top;
		std::string result;
		if (w > 0 && h > 0)
		{
			CImage image; image.Create(w, h, 32);
			HDC hdc = GetDC(nullptr);
			if (BitBlt(image.GetDC(), 0, 0, w, h, hdc, rect.left, rect.top, SRCCOPY)) result = OnnxOcr::scan(image);
			image.ReleaseDC();
			ReleaseDC(nullptr, hdc);
		}
		return result;
	}
};