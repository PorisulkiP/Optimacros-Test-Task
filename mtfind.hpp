// mtfind.cpp : осуществляет поиск подстроки в текстовом файле по маске.
// Запуск программы: mtfind input.txt "?ad"
// Результат: 3; 5 bad; 6 mad; 7 had.

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <future>
#include <vector>
#include <mutex>
#include <map>

std::mutex g_mutex;
int main_1(int argc, const char* argv[]);

int main() 
{
    const char* argv[] = { "C:\\Users\\alex1\\projects\\test2.txt", "?ad"};
    
    main_1(2, argv);
    return 0;
}

std::string comparisonWords(const std::string& word, const std::string& mask)
{   /*  
    *   алло | ?ло \            алло | ?да  \
    *   ?ло   ?ло   - "лло"     ?да   ?да    - ""
    *   алло алло  /            алло алло   /
    */
    for (size_t wI = 0; wI < word.length(); ++wI) // wI - word index
    {        
        size_t mICount = 0;
        std::string res;
        for (size_t mI = 0; mI < mask.length(); ++mI) // mI - mask index
        {
            if (wI + mI < word.length())
            {
                if ((mask[mI] == '?') || (word[wI + mI] == mask[mI]))
                {
                    res += word[wI + mI];
                    ++mICount;
                }
            }
            else { break; }
        }
        
        if (mICount == mask.length()) { return res; }
    }
    return "";
}

int main_1(int argc, const char* argv[]) // в argv получаются входные данные
{
    if (argc == 2) // если передаем аргументы, то argc будет больше 1
    {
        std::ifstream in(argv[0]); // окрываем файл для чтения
        std::vector<std::string> dataFromFile;
        std::map<std::string, std::string> answer;
        std::string mask = argv[1];
        std::vector<std::thread> threads; // Создаётся вектор потоков

        if (in.is_open())
        {
            std::string line;
            while (getline(in, line))
            {
                dataFromFile.emplace_back(line); // Данные из файла записываются в вектор
            }
        }
        in.close(); // закрываем файл
        
        uint32_t operationCount = dataFromFile.size() - mask.length();
        uint32_t threadCount = std::thread::hardware_concurrency();
        std::thread comparationThread;

        for (size_t i = 0; i < threadCount; ++i)
        {            
            std::thread comparationThread = std::thread([=, &answer, &dataFromFile] 
                {
                // TODO: При недоборе данных в dataFromFile.at(it) под размер it += threadCount
                // выделить столько потоков, сколько осталось данных
                for (size_t it = i; it <= operationCount; it += threadCount)
                {
                    if (dataFromFile.at(it).empty()) { continue; }
                    // Получается всё первое значение
                    char* next_token = NULL;
                    std::string num = strtok_s(dataFromFile.at(it).data(), " ", &next_token);
                    std::istringstream ist(dataFromFile.at(it));
                    std::string word;
                    while (ist >> word)
                    {
                        if (word.length() < mask.length()) { continue; }
                        bool flag = true;
                        std::string tmpAns = comparisonWords(word, mask);

                        if (!tmpAns.empty())
                        {
                            for (auto& itAns : answer)
                            {                                
                                if (itAns.second == tmpAns)
                                {
                                    flag = false;
                                    break;
                                }
                            }
                            if(flag)
                            {      
                                std::lock_guard<std::mutex> lk(g_mutex);
                                answer.insert(std::make_pair(num, tmpAns));
                            }
                        }
                    }
                }
            });
            threads.emplace_back(std::move(comparationThread)); // Поток добавляется в вектор потоков
        }                
        
        // Объединение всех потоков
        for (auto&& thr : threads)
        {            
            if (thr.joinable())
            {
                thr.join();
            }
        }

        
        std::cout << answer.size() << std::endl; // Вывод кол-ва найдённых элементов
        
        for (auto& word : answer) // Вывод итога в консоль
        {
            std::cout << word.first << " " << word.second << std::endl;
        }
        
    }
    else  
    { 
        std::cout << "Неправильно введены аргументы" << std::endl; 
        system("pause");
        return 1;
    }

    system("pause");
    return 0;
}

