#include <iostream>
#include <string>
#include "server.h" 
#include "sql_parser.h"
#include "schema_creator.h"
#include "capture.h"

// Функция для обработки SQL-запросов
std::string process_query(const std::string& sql_query) {
    std::stringstream ss(sql_query);
    std::string command;
    ss >> command;

    if (command == "INSERT") {
        OutputCapturer capturer; // Создаем объект для перехвата вывода
        bool success = executeInsert(sql_query); // Выполняем INSERT

        std::string select_result = capturer.getCapturedOutput(); // Получаем перехваченный вывод

        if (success) {
             return "INSERT_RESULT:" + select_result; // Возвращаем результат с префиксом
        } else {
             return "Ошибка выполнения запроса INSERT.\n"; 
        }

    } else if (command == "DELETE") {
                OutputCapturer capturer; // Создаем объект для перехвата вывода
        bool success = executeDelete(sql_query); // Выполняем DELETE

        std::string select_result = capturer.getCapturedOutput(); // Получаем перехваченный вывод

        if (success) {
             return "DELETE_RESULT:" + select_result; // Возвращаем результат с префиксом
        } else {
             return "Ошибка выполнения запроса DELETE.\n"; 
        }

    }
    else if (command == "SELECT") {
        OutputCapturer capturer; // Создаем объект для перехвата вывода
        bool success = executeSelect(sql_query); // Выполняем SELECT

        std::string select_result = capturer.getCapturedOutput(); // Получаем перехваченный вывод

        if (success) {
             return "SELECT_RESULT:" + select_result; // Возвращаем результат с префиксом
        } else {
             return "Ошибка выполнения запроса SELECT.\n"; 
        }

    }

    return "Ошибка: неизвестный тип запроса.\n";
}

int main() {
    if (!loadSchema("schema.json")) {
        std::cerr << "SERVER ERROR: Ошибка загрузки схемы.\n";
        return 1;
    }

    std::string schema_name = schema_data["name"];
    if (!createSchema("schema.json", schema_name)) {
        std::cerr << "SERVER ERROR: Ошибка создания схемы.\n";
        return 1;
    }

    start_server(); 

    return 0;
}
