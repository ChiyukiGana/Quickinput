#pragma once
#include <vector>
#include <string>
#include <algorithm>

namespace QiTools
{
    class CsvTool {
    public:
        using Csv = std::vector<std::vector<std::string>>;

    private:
        Csv m_csv;

    public:
        CsvTool() = default;

        explicit CsvTool(const std::string& csv) {
            m_csv = parse(csv);
        }

        std::vector<std::string> operator[](size_t row) const {
            return m_csv[row];
        }

        std::vector<std::string>& operator[](size_t row) {
            return m_csv[row];
        }

        std::string at(size_t row, size_t col) const {
            return m_csv[row][col];
        }

        std::string& at(size_t row, size_t col) {
            return m_csv[row][col];
        }

        std::string read(size_t row, size_t col) const {
            if (row < m_csv.size() && col < m_csv[row].size()) return m_csv[row][col];
            return {};
        }

        std::string write(size_t row, size_t col, const std::string& data) {
            if (row < m_csv.size() && col < m_csv[row].size()) {
                m_csv[row][col] = data;
                return data;
            }
            return {};
        }

        std::string toString() const {
            return toString(m_csv);
        }

        static std::string read(const std::string& csv, size_t row, size_t col) {
            Csv csvm = parse(csv);
            if (row < csvm.size() && col < csvm[row].size()) return csvm[row][col];
            return {};
        }

        static std::string write(const std::string& csv, size_t row, size_t col, const std::string& data) {
            Csv csvm = parse(csv);
            if (row < csvm.size() && col < csvm[row].size()) {
                csvm[row][col] = data;
                return toString(csvm);
            }
            return csv;
        }

        static std::string toString(const Csv& csv) {
            std::string result;
            for (size_t i = 0; i < csv.size(); ++i) {
                const auto& row = csv[i];
                for (size_t j = 0; j < row.size(); ++j) {
                    bool needsQuotes = false;
                    std::string cell = row[j];

                    if (cell.find_first_of(",\"\n") != std::string::npos) {
                        needsQuotes = true;
                    }

                    if (needsQuotes) {
                        cell = "\"" + std::string(cell) + "\"";
                        size_t pos = 0;
                        while (pos = cell.find('\"', pos)) {
                            if (pos == std::string::npos) break;
                            cell.insert(pos, "\"");
                            pos += 2;
                        }
                    }

                    if (j > 0) result += ',';
                    result += cell;
                }
                if (i < csv.size() - 1) result += '\n';
            }
            return result;
        }

        static Csv parse(const std::string& csv) {
            Csv result;
            std::vector<std::string> currentRow;
            std::string currentCell;
            enum State { Normal, InQuotes, AfterClosingQuote };
            State state = Normal;

            for (size_t i = 0; i < csv.size(); ++i) {
                char c = csv[i];
                switch (state) {
                case Normal:
                    if (c == ',') {
                        currentRow.push_back(currentCell);
                        currentCell.clear();
                    }
                    else if (c == '\r' || c == '\n') {
                        currentRow.push_back(currentCell);
                        currentCell.clear();
                        if (!currentRow.empty()) {
                            result.push_back(currentRow);
                            currentRow.clear();
                        }
                        if (c == '\r' && i + 1 < csv.size() && csv[i + 1] == '\n') ++i;
                    }
                    else if (c == '"' && currentCell.empty()) {
                        state = InQuotes;
                    }
                    else {
                        currentCell += c;
                    }
                    break;

                case InQuotes:
                    if (c == '"') {
                        if (i + 1 < csv.size() && csv[i + 1] == '"') {
                            currentCell += '"';
                            ++i;
                        }
                        else {
                            state = AfterClosingQuote;
                        }
                    }
                    else {
                        currentCell += c;
                    }
                    break;

                case AfterClosingQuote:
                    if (c == ',') {
                        state = Normal;
                        currentRow.push_back(currentCell);
                        currentCell.clear();
                    }
                    else if (c == '\r' || c == '\n') {
                        state = Normal;
                        currentRow.push_back(currentCell);
                        currentCell.clear();
                        if (!currentRow.empty()) {
                            result.push_back(currentRow);
                            currentRow.clear();
                        }
                        if (c == '\r' && i + 1 < csv.size() && csv[i + 1] == '\n') ++i;
                    }
                    break;
                }
            }

            if (state == InQuotes || !currentCell.empty()) {
                currentRow.push_back(currentCell);
            }
            if (!currentRow.empty()) {
                result.push_back(currentRow);
            }

            return result;
        }
    };
}