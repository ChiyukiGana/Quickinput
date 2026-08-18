#ifndef CG_FORMAT_H
#define CG_FORMAT_H
#include <map>
#include <qstring.h>
#include <qbytearray.h>
namespace Format
{
	static std::map<QString, QString> pairmap(const char* data, const size_t size, char note_ch = {}, char connect_ch = {})
	{
		std::map<QString, QString> map;
		for (size_t i = 0; i < size;)
		{
			const char ch = data[i];
			if (note_ch && ch == note_ch)
			{
				for (; i < size && (data[i] != '\r' && data[i] != '\n' && data[i] != '\0'); i++);
			}
			else if (ch != '\r' && ch != '\n' && ch != '\0')
			{
				const size_t key_begin = i;
				for (; i < size && (data[i] != '=' && data[i] != '\r' && data[i] != '\n' && data[i] != '\0'); i++);
				const size_t key_size = i - key_begin;

				if (data[i] != '=' || !key_size)
				{
					for (; i < size && (data[i] == '\r' || data[i] == '\n' || data[i] == '\0'); i++);
					continue;
				}

				i++;
				if (i >= size) break;

				if (connect_ch)
				{
					QString val;
					bool connect = true;
					while (connect)
					{
						const size_t val_begin = i;
						size_t bs{};
						for (; i < size && (data[i] != '\r' && data[i] != '\n' && data[i] != '\0'); i++) if (data[i] == connect_ch) bs = i;
						connect = bs && i - bs == 1;
						const size_t val_size = connect ? bs - val_begin : i - val_begin;
						if (val_size)
						{
							const bool bs2 = bs > 0 && data[bs - 1] == connect_ch;
							val.append(QString::fromUtf8(data + val_begin, val_size - (bs2 ? 1 : 0)));
							if (bs2) val.push_back('\n'), i--;
						}
						for (; i < size && data[i] == connect_ch; i++);
						for (char prev{}; i < size && (data[i] == '\r' || data[i] == '\n' || data[i] == '\0') && prev != data[i]; prev = data[i], i++);
					}
					if (key_size && val.size()) map.insert(std::make_pair(QString::fromUtf8(data + key_begin, key_size), std::move(val)));
				}
				else
				{
					const size_t val_begin = i;
					for (; i < size && (data[i] != '\r' && data[i] != '\n' && data[i] != '\0'); i++);
					const size_t val_size = i - val_begin;
					if (key_size && val_size) map.insert(std::make_pair(QString::fromUtf8(data + key_begin, key_size), QString::fromUtf8(data + val_begin, val_size)));
				}
			}
			else
			{
				i++;
			}
		}
		return map;
	}
}
#endif