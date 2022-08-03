// mtfind.cpp : осуществляет поиск подстроки в текстовом файле по маске.
// Запуск программы: mtfind input.txt "?ad"
// Результат: 3; 5 bad; 6 mad; 7 had.

#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <future>
#include <vector>
#include <mutex>
#include <deque>

std::mutex g_mutex;

// Фнкция сравнивает две пары строк.
// Пример:
//    std::sort(elems.begin(), elems.end(), valComp);
inline
bool valComp(const std::pair<std::string, std::string>& a,
			 const std::pair<std::string, std::string>& b)
{
	return std::stoll(a.second) < std::stoll(b.second);
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
		std::string res; // Итоговая строка
		size_t mICount = 0; // Колличество совпадений по по маске		
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
	return std::string(); // RVO оптимизация
}

int main(int argc, const char* argv[]) // в argv получаются входные данные
{
	if (argc == 3) // Если передаются аргументы, то argc будет больше 1
	{
		std::ifstream in(argv[1]); // окрываем файл для чтения
		std::deque<std::string> dataFromFile;
		std::unordered_map<std::string, std::string> answer;
		const std::string mask = argv[2];

		if (in.is_open())
		{
			std::string line;
			std::vector<std::thread> threads; // Создаётся вектор потоков
			const size_t threadCount = std::thread::hardware_concurrency();
			const size_t linesRead = threadCount * sizeof(std::string) * 1000;
			while (!in.eof())
			{
				// Из файла получаются данные
				for (size_t i = 0; i < linesRead && !in.eof(); ++i)
				{
					getline(in, line);
					if (line.empty()) { continue; }
					dataFromFile.emplace_back(line);
				}

				for (size_t i = 0; i < threadCount; ++i)
				{
					auto comparationThread = std::thread([=, &answer, &dataFromFile]
					{
						for (size_t it = i; it < dataFromFile.size(); it += threadCount)
						{
							auto localLine = dataFromFile.at(it);

							// Получается номер строки
							std::istringstream iss(localLine);
							std::string num;
							std::getline(iss, num, ' ');
							
							localLine.erase(0, num.length()); // Удаление числа из начала строки
							std::istringstream ist(localLine);
							std::string word;
							
							while (ist >> word) // Из каждого предложения достаются слова
							{
								if (word.length() < mask.length()) { continue; }
								// Слова сравниваются
								std::string tmpAns = comparisonWords(std::move(word), mask);
								if (tmpAns.empty()) { continue; }

								bool flag = true;
								for (auto& itAns : answer)
								{
									if (itAns.first == tmpAns)
									{
										flag = false;
										break;
									}
								}
								// TODO: избавиться от g_mutex и возвращать данные через promise
								if (flag) // Если слово не повторяется, то добавляется в map
								{
									std::lock_guard<std::mutex> lk(g_mutex);
									// num без std::move, поскольку может повторяться
									answer.insert(std::make_pair(std::move(tmpAns), num));
								}
							}
						}
					});
					threads.emplace_back(std::move(comparationThread)); // Поток добавляется в вектор потоков
				}

				// Объединение всех потоков
				for (auto&& thr : threads)
				{
					if (thr.joinable()) { thr.join(); }
				}
				threads.clear();
				dataFromFile.clear();
			}
		}
		in.close(); // Поток чтения файла закрывается
		
		std::thread clearFileCashThread = std::thread([&] { dataFromFile.clear(); });

		// Для сортировки элементов по ключам
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