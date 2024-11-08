#include "schema_creator.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>
#include "json.hpp"  

namespace fs = std::filesystem;
using json = nlohmann::json;

bool createSchema(const std::string& schema_filename, const std::string& schema_name) {

    std::ifstream schema_file(schema_filename);
    if (!schema_file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл " << schema_filename << std::endl;
        return false;
    }

    json schema_data;
    try {
        schema_file >> schema_data;
    } catch (const json::parse_error& e) {
        std::cerr << "Ошибка парсинга JSON: " << e.what() << std::endl;
        return false;
    }

    if (schema_name.empty()) {
        std::cerr << "Ошибка: не указано имя схемы в " << schema_filename << std::endl;
        return false;
    }

    if (!fs::exists(schema_name)) {
        std::cout << "Папка схемы отсутствует, создаём: " << schema_name << std::endl;
        fs::create_directory(schema_name);
    }

    int tuples_limit = schema_data["tuples_limit"];
    int current_pk = 0;  // Счётчик первичного ключа

    for (const auto& [table_name, columns] : schema_data["structure"].items()) {
        std::string table_path = schema_name + "/" + table_name;

        if (!fs::exists(table_path)) {
            std::cout << "Папка таблицы отсутствует, создаём: " << table_path << std::endl;
            fs::create_directory(table_path);
        }

        std::string csv_filename = table_path + "/1.csv";
        if (!fs::exists(csv_filename)) {
            std::cout << "Файл 1.csv отсутствует, создаём: " << csv_filename << std::endl;
            std::ofstream csv_file(csv_filename);
            if (!csv_file.is_open()) {
                std::cerr << "Ошибка: не удалось создать CSV файл для " << table_name << std::endl;
                return false;
            }

            // Записываем заголовки столбцов
            csv_file << table_name << "_pk,";  // Столбец первичного ключа
            for (size_t i = 0; i < columns.size(); ++i) {
                csv_file << columns[i].get<std::string>();
                if (i < columns.size() - 1) {
                    csv_file << ",";
                }
            }
            csv_file << std::endl;
            csv_file.close();
        }

        std::string pk_sequence_filename = table_path + "/" + table_name + "_pk_sequence";
        if (!fs::exists(pk_sequence_filename)) {
            std::cout << "Файл " << table_name << "_pk_sequence отсутствует, создаём: " << pk_sequence_filename << std::endl;
            std::ofstream pk_file(pk_sequence_filename);
            if (!pk_file.is_open()) {
                std::cerr << "Ошибка: не удалось создать файл первичного ключа для " << table_name << std::endl;
                return false;
            }
            pk_file << current_pk << std::endl;  // Записываем текущее значение первичного ключа
            pk_file.close();
        }

        std::string locked_filename = table_path + "/locked";
        if (!fs::exists(locked_filename)) {
            std::cout << "Файл locked отсутствует, создаём: " << locked_filename << std::endl;
            std::ofstream locked_file(locked_filename);
            if (!locked_file.is_open()) {
                std::cerr << "Ошибка: не удалось создать файл locked для " << table_name << std::endl;
                return false;
            }
            locked_file << "0" << std::endl;  // Статус блокировки по умолчанию (0 — не заблокировано)
            locked_file.close();
        }
    }

    std::cout << "Схема успешно создана/проверена." << std::endl;
    return true;
}