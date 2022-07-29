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
#include <unordered_map>
#include <deque>

std::mutex g_mutex;

// Фнкция сравнивает две пары строк.
// Пример:
//    std::sort(elems.begin(), elems.end(), valComp);
inline
bool valComp(const std::pair<std::string, std::string> a,
			 const std::pair<std::string, std::string> b)
{
	return a.second < b.second;
}

// Фнкция сравнивает слово по маске.
// Пример:
//    std::string tmpAns = comparisonWords(word, mask);
std::string comparisonWords(const std::string& word, const std::string& mask)
{   /*
	*   алло | ?ло \            алло | ?да  \
	*   ?ло   ?ло   - "лло"     ?да   ?да    - ""
	*   алло алло  /            алло алло   /
	*/

	for (size_t wI = 0; wI < word.length(); ++wI) // wI - word index
	{
		size_t mICount = 0; // Колличество совпадений по по маске
		std::string res; // Итоговая строка
		for (size_t mI = 0; mI < mask.length(); ++mI) // mI - mask index
		{
			if (wI + mI < word.length())
			{
				// если знак из маски это ?, то подойдёт любой символ из строки
				if ((mask[mI] == '?') || (word[wI + mI] == mask[mI]))
				{
					res += word[wI + mI];
					++mICount;
				}
			}
			else { break; }
		}
		// Когда пол-во совпадений по маске равно кол-ву элементов в маске,
		// значит строка полностью подходит под маску
		if (mICount == mask.length()) { return res; }
	}
	return "";
}

int main(int argc, const char* arg[]) // в argv получаются входные данные
{	
	argc = 3;
	const char* argv[] = {"qwerty", "C:\\Users\\alex1\\projects\\test.txt", "?ad" };
	if (argc == 3) // Если передаются аргументы, то argc будет больше 1
	{
		std::ifstream in(argv[1]); // окрываем файл для чтения
		std::deque<std::string> dataFromFile;
		std::string mask = argv[2];

		if (in.is_open())
		{
			std::string line;
			while (getline(in, line))
			{
				dataFromFile.emplace_back(std::move(line)); // Данные из файла записываются в вектор
			}
		}
		in.close(); // закрываем файл

		std::unordered_map<std::string, std::string> answer;
		std::vector<std::thread> threads; // Создаётся вектор потоков
		uint16_t threadCount = std::thread::hardware_concurrency();

		for (size_t i = 0; i < threadCount; ++i)
		{
			std::thread comparationThread = std::thread([=, &answer, &dataFromFile]
				{
					for (size_t it = i; it < dataFromFile.size(); it += threadCount)
					{
						std::string line = dataFromFile.at(it);
						if (line.empty()) { continue; }
						// Получается всё первое число в виде строки, 
						// чтобы не было ограничений на ко-во символов
						
						char* next_token = NULL; // для безопасности strtok_s
						char* dup = _strdup(line.c_str());
						if (dup == NULL)
						{
							std::cout << "Система не может выделить столько памяти" << std::endl;
							system("pause");
							return 1;
						}
						std::string num = strtok_s(dup, " ", &next_token);
						free(dup);
						line.erase(0, num.length()); // Удаление числа из начала строки
						std::istringstream ist(line);
						std::string word;
						// Из каждого предложения достаются слова
						while (ist >> word)
						{
							if (word.length() < mask.length()) { continue; }

							std::string tmpAns = comparisonWords(word, mask);
							if (!tmpAns.empty())
							{
								bool flag = true;
								for (auto& itAns : answer)
								{

									if (itAns.first == tmpAns)
									{
										flag = false;
										break;
									}
								}
								if (flag) // Если слово не повторяется, то добавляется в map
								{
									std::lock_guard<std::mutex> lk(g_mutex);
									// num без std::move, поскольку может повторяться
									answer.insert(std::make_pair(std::move(tmpAns), num));
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

		std::thread clearFileCashThread = std::thread([&]
			{
				dataFromFile.clear();
			});

		std::vector<std::pair<std::string, std::string>> elems(answer.begin(), answer.end());

		// Полученные элементы сортируются по значению
		std::thread sortThread = std::thread([&]
			{
				std::sort(elems.begin(), elems.end(), valComp); 
			});
		
		std::thread([&] { answer.clear(); }).detach();

		sortThread.join();
		clearFileCashThread.join();

		std::cout << elems.size() << std::endl; // Вывод кол-ва найденных элементов
		for (auto& word : elems) // Вывод вектора в консоль
		{
			std::cout << word.second << " " << word.first << std::endl;
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