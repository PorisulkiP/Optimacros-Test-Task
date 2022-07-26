// mtfind.cpp : осуществляет поиск подстроки в текстовом файле по маске.
// Запуск программы: mtfind input.txt "?ad"
// Результат: 3; 5 bad; 6 mad; 7 had.

#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <mutex>
#include <list>

#define _CRT_NONSTDC_NO_DEPRECATE
#pragma warning(disable : 4996)

int main_1(int argc, const char* argv[]);


int main() {
    const char* argv[] = { "C:\\Users\\alex1\\projects\\test.txt", "?ad"};
    
    main_1(2, argv);
    return 0;
}

void comparisonWords(std::string& word, std::string& mask, bool& flag) 
{   /*  
    *   алло | ?ло \            алло | ?да  \
    *   ?ло   ?ло   - true      ?да   ?да    - false
    *   алло алло  /            алло алло   /
    */
    for (size_t wI = 0; wI < word.length(); ++wI) // wI - word index
    {
        size_t mICount = 0;
        std::string answer;
        for (size_t mI = 0; mI < mask.length(); ++mI) // mI - mask index
        {
            if (wI + mI < word.length())
            {
                if ((mask[mI] == '?') || (word[wI + mI] == mask[mI]))
                {
                    answer += word[wI + mI];
                    ++mICount; 
                }
            }
            else { break; }
        }
        if (mICount == mask.length()) 
        {
            word = answer;
            flag = true;
        }
    }
}

int main_1(int argc, const char* argv[]) // в argv получаются входные данные
{
    //setlocale(LC_ALL, "Russian");
    if (argc >= 2) // Если переданы аргументы
    {
        std::ifstream in(argv[0]); // окрываем файл для чтения
        if (in.is_open())
        {
            std::list<std::string> words;
            std::string line, mask = argv[1];
            std::future<void> future;
            std::vector<std::thread> threads; // Создаётся вектор потоков
            std::mutex gMutex;
            while (getline(in, line))
            {                
                std::istringstream ist(line);                
                // Создаётся максимальное число потоков для выполнения задачи
                for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i)
                {
                    std::thread thr([&] {                        
                        std::string tmp;
                        while (ist >> tmp) {
                            bool flag = false;
                            if (tmp.length() < mask.length()) { continue; }
                            std::lock_guard<std::mutex> lock(gMutex);
                            comparisonWords(tmp, mask, flag);
                            if (flag) { words.push_back(tmp); }
                        }
                        });
                    // Поток добавляется в вектор
                    threads.emplace_back(std::move(thr));
                }
                for (auto& thr : threads) { 
                    if (thr.joinable())
                        thr.join(); 
                }
            }
            for (auto& word : words)
            {
                std::cout << word << std::endl;
            }
        }
        
        in.close(); // закрываем файл
    }
    else  // если передаем аргументы, то argc будет больше 1(в зависимости от кол-ва аргументов)
    { 
        std::cout << "Неправильно введены аргументы" << std::endl; 
        system("pause");
        return 1;
    }
    system("pause");
    return 0;
}

