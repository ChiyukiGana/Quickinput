#pragma once
#ifndef RES_MONITOR
#define RES_MONITOR

#include <pdh.h>
#include <pdhmsg.h>
#include <time.h>
#include <string>
#include <sstream>
#include <thread>
#include <atomic>
#include <mutex>
#include <optional>
#include <dxgi.h>
#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "dxgi.lib")

class ResMonitor
{
	struct CPU_INFO
	{
		std::wstring name;
		uint32_t base_clock = 0;
		uint32_t memory = 0;
	};
	struct GPU_INFO
	{
		std::wstring name;
		uint32_t memory = 0;
	};

	enum ValueType
	{
		_avg,
		_max,
		_sum,
		_first
	};

	static constexpr const wchar_t* c_path_cpu_usage = L"\\Processor Information(_Total)\\% Processor Utility";
	static constexpr const wchar_t* c_path_cpu_clock = L"\\Processor Information(_Total)\\Actual Frequency";
	static constexpr const wchar_t* c_path_cpu_base_clock = L"\\Processor Information(_Total)\\Processor Frequency";
	static constexpr const wchar_t* c_path_gpu_usage = L"\\GPU Engine(*_3D)\\Utilization Percentage";
	static constexpr const wchar_t* c_path_gpu_mem_usage = L"\\GPU Adapter Memory(*)\\Dedicated Usage";
	static constexpr const wchar_t* c_path_disk_read = L"\\PhysicalDisk(_Total)\\Disk Read Bytes/sec";
	static constexpr const wchar_t* c_path_disk_write = L"\\PhysicalDisk(_Total)\\Disk Write Bytes/sec";
	static constexpr const wchar_t* c_path_network_receive = L"\\Network Interface(*)\\Bytes Received/sec";
	static constexpr const wchar_t* c_path_network_send = L"\\Network Interface(*)\\Bytes Sent/sec";

	std::atomic_bool m_stop{};
	std::atomic_uint m_interval = 1000;
	std::thread m_thread;
	std::mutex m_thread_mutex;
	std::mutex m_query_mutex;
	HQUERY m_query{};

	HCOUNTER m_count_cpu_usage{};
	HCOUNTER m_count_cpu_clock{};
	HCOUNTER m_count_cpu_base_clock{};
	HCOUNTER m_count_gpu_usage{};
	HCOUNTER m_count_gpu_mem_usage{};
	HCOUNTER m_count_disk_read{};
	HCOUNTER m_count_disk_write{};
	HCOUNTER m_count_network_receive{};
	HCOUNTER m_count_network_send{};

	void sleep(clock_t ms) const
	{
		const clock_t end = clock() + ms;
		while (!m_stop && end > clock()) Sleep(16);
	}
	bool setCounter(const wchar_t* path, HCOUNTER& count, bool remove)
	{
		if (!m_init) return false;
		std::unique_lock<std::mutex> _(m_query_mutex);
		if (remove)
		{
			if (count)
			{
				if (PdhRemoveCounter(count) != ERROR_SUCCESS) return false;
				count = nullptr;
			}
		}
		else
		{
			if (!count) return PdhAddCounterW(m_query, path, NULL, &count) == ERROR_SUCCESS;
		}
		return true;
	}
	bool queryCounter(const HCOUNTER& count, double& value)
	{
		PDH_FMT_COUNTERVALUE countValue{};
		if (PdhGetFormattedCounterValue(count, PDH_FMT_DOUBLE, NULL, &countValue) == ERROR_SUCCESS)
		{
			value = countValue.doubleValue;
			return true;
		}
		return false;
	}
	bool queryCounterArray(const HCOUNTER& count, double& value, ValueType type)
	{
		std::unique_lock<std::mutex> _(m_query_mutex);
		DWORD bytes{}, itemCount{};
		if (PdhGetFormattedCounterArrayW(count, PDH_FMT_DOUBLE, &bytes, &itemCount, nullptr) != PDH_MORE_DATA) return false;

		auto __ = std::make_unique<char[]>(bytes);
		auto items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(__.get());

		PdhGetFormattedCounterArrayW(count, PDH_FMT_DOUBLE, &bytes, &itemCount, items);
		if (itemCount < 1) return false;
		value = {};
		if (type == _max)
		{
			for (DWORD i = 0; i < itemCount; i++) if (items[i].FmtValue.doubleValue > value) value = items[i].FmtValue.doubleValue;
		}
		else if (type == _sum)
		{
			for (DWORD i = 0; i < itemCount; i++) value += items[i].FmtValue.doubleValue;
		}
		else if (type == _first)
		{
			value = items->FmtValue.doubleValue;
		}
		else
		{
			for (DWORD i = 0; i < itemCount; i++) value += items[i].FmtValue.doubleValue;
			value /= itemCount;
		}
		return true;
	}
public:
	const bool m_init;
	ResMonitor() : m_query(nullptr), m_init(PdhOpenQueryW(NULL, 0, &m_query) == ERROR_SUCCESS) {}
	~ResMonitor()
	{
		if (!m_init) return;
		stop();
		PdhCloseQuery(m_query);
	}

	bool setCpuUsage(bool query) { return setCounter(c_path_cpu_usage, m_count_cpu_usage, !query); }
	bool setCpuClock(bool query) { return setCounter(c_path_cpu_clock, m_count_cpu_clock, !query); }
	bool setCpuBaseClock(bool query) { return setCounter(c_path_cpu_base_clock, m_count_cpu_base_clock, !query); }
	bool setGpuUsage(bool query) { return setCounter(c_path_gpu_usage, m_count_gpu_usage, !query); }
	bool setGpuMemoryUsage(bool query) { return setCounter(c_path_gpu_mem_usage, m_count_gpu_mem_usage, !query); }
	bool setDisk(bool query) { return setCounter(c_path_disk_read, m_count_disk_read, !query) && setCounter(c_path_disk_write, m_count_disk_write, !query); }
	bool setNetwork(bool query) { return setCounter(c_path_network_receive, m_count_network_receive, !query) && setCounter(c_path_network_send, m_count_network_send, !query); }
	void setInterval(uint32_t interval = 1000) { m_interval = interval > 1000 ? interval : 1000; }
	uint32_t interval() const { return m_interval; }

	std::optional<double> queryCpuUsage()
	{
		double value{};
		if (queryCounter(m_count_cpu_usage, value)) return std::clamp(value, 0.0, 100.0);
		return std::nullopt;
	}
	std::optional<double> queryCpuClock()
	{
		double value{};
		if (queryCounter(m_count_cpu_clock, value)) return value;
		return std::nullopt;
	}
	std::optional<double> queryCpuBaseClock()
	{
		double value{};
		if (queryCounter(m_count_cpu_base_clock, value)) return value;
		return std::nullopt;
	}
	std::optional<double> queryGpuUsage()
	{
		double value{};
		if (queryCounterArray(m_count_gpu_usage, value, _max)) return std::clamp(value, 0.0, 100.0);
		return std::nullopt;
	}
	std::optional<double> queryGpuMemoryUsage()
	{
		double value{};
		if (queryCounterArray(m_count_gpu_mem_usage, value, _max)) return value;
		return std::nullopt;
	}
	std::optional<double> queryDiskRead()
	{
		double value{};
		if (queryCounter(m_count_disk_read, value)) return value;
		return std::nullopt;
	}
	std::optional<double> queryDiskWrite()
	{
		double value{};
		if (queryCounter(m_count_disk_write, value)) return value;
		return std::nullopt;
	}
	std::optional<double> queryNetworkReceive()
	{
		double value{};
		if (queryCounterArray(m_count_network_receive, value, _avg)) return value;
		return std::nullopt;
	}
	std::optional<double> queryNetworkSend()
	{
		double value{};
		if (queryCounterArray(m_count_network_send, value, _avg)) return value;
		return std::nullopt;
	}

	double cpuUsage()
	{
		auto value = queryCpuUsage();
		if (value) return *value;
		return 0.0;
	}
	double cpuClock()
	{
		auto value = queryCpuClock();
		if (value) return *value;
		return 0.0;
	}
	double cpuBaseClock()
	{
		auto value = queryCpuBaseClock();
		if (value) return *value;
		return 0.0;
	}
	double gpuUsage()
	{
		auto value = queryGpuUsage();
		if (value) return *value;
		return 0.0;
	}
	double gpuMemoryUsage()
	{
		auto value = queryGpuMemoryUsage();
		if (value) return *value;
		return 0.0;
	}
	double diskRead()
	{
		auto value = queryDiskRead();
		if (value) return *value;
		return 0.0;
	}
	double diskWrite()
	{
		auto value = queryDiskWrite();
		if (value) return *value;
		return 0.0;
	}
	double networkReceive()
	{
		auto value = queryNetworkReceive();
		if (value) return *value;
		return 0.0;
	}
	double networkSend()
	{
		auto value = queryNetworkSend();
		if (value) return *value;
		return 0.0;
	}
	static std::optional<MEMORYSTATUSEX> memoryInfo()
	{
		MEMORYSTATUSEX status{ sizeof(MEMORYSTATUSEX) };
		if (GlobalMemoryStatusEx(&status) != FALSE) return status;
		return std::nullopt;
	}
	static double memoryTotal()
	{
		auto info = memoryInfo();
		if (info) return info->ullTotalPhys;
		return 0.0;
	}
	static double memoryUsage()
	{
		auto info = memoryInfo();
		if (info) return info->ullTotalPhys - info->ullAvailPhys;
		return 0.0;
	}
	static double memoryFree()
	{
		auto info = memoryInfo();
		if (info) return info->ullAvailPhys;
		return 0.0;
	}
	static std::optional<DXGI_ADAPTER_DESC> gpuInfo()
	{
		IDXGIFactory* factory = nullptr;
		if (FAILED(CreateDXGIFactory(IID_PPV_ARGS(&factory)))) return std::nullopt;
		IDXGIAdapter* adapter;
		if (FAILED(factory->EnumAdapters(0, &adapter)))
		{
			factory->Release();
			return std::nullopt;
		}
		factory->Release();
		DXGI_ADAPTER_DESC desc{};
		if (FAILED(adapter->GetDesc(&desc)))
		{
			adapter->Release();
			return std::nullopt;
		}
		adapter->Release();
		return desc;
	}
	static double gpuMemoryTotal()
	{
		auto info = gpuInfo();
		if (info) return info->DedicatedVideoMemory;
		return 0.0;
	}

	bool init() const { return m_init; }
	bool running() const { return m_thread.joinable(); }
	bool start()
	{
		if (!m_init) return false;
		{
			std::unique_lock<std::mutex> _(m_query_mutex);
			if (PdhCollectQueryData(m_query) != ERROR_SUCCESS) return false;
			sleep(100);
			PdhCollectQueryData(m_query);
		}
		std::unique_lock<std::mutex> _(m_thread_mutex);
		if (m_thread.joinable()) return true;
		m_stop = false;
		m_thread = std::thread([this]() {
			while (!m_stop)
			{
				sleep(m_interval);
				{
					std::unique_lock<std::mutex> _(m_query_mutex);
					PdhCollectQueryData(m_query);
				}
			}
			});
		return true;
	}
	bool stop()
	{
		if (!m_init) return false;
		std::unique_lock<std::mutex> _(m_thread_mutex);
		if (!m_thread.joinable()) return true;
		m_stop = true;
		m_thread.join();
		return true;
	}

	static std::string toString(double value, int l = 2)
	{
		std::stringstream ss;
		ss << std::setprecision(l) << std::fixed << value;
		return ss.str();
	}
	static std::string toSize(size_t bytes, int l = 2)
	{
		if (bytes < 1024) return std::to_string(bytes) + "B";
		if (bytes < 1024 * 1024) return std::to_string(bytes / 1024) + "KB";
		if (bytes < 1024 * 1024 * 1024) return toString(static_cast<double>(bytes) / (1024.0 * 1024.0), l) + "MB";
		return toString(static_cast<double>(bytes) / (1024.0 * 1024.0 * 1024.0), l) + "GB";
	}
};

#endif