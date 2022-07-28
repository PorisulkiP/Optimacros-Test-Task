# Optimacros-Test-Task

<p>
Написать программу mtfind, производящую поиск подстроки в текстовом файле по маске с использованием многопоточности. <br>
Маска - это строка, где "?" обозначает любой символ.

Программа принимает в качестве параметров командной строки:
1) Имя текстового файла, в котором должен идти поиск (размер файла - до 1Гб). <br>
2) Маску для поиска, в кавычках. Максимальная длина маски 100 символов. <br>

Вывод программы должен быть в следующем формате:
- На первой строке - количество найденных вхождений. <br>
- Далее информация о каждом вхождении, каждое на отдельной строке, через пробел: <br>
номер строки, позиция в строке, само найденное вхождение.

Дополнения:
- В текстовом файле кодировка только 7-bit ASCII
- Поиск с учетом регистра
- Каждое вхождение может быть только на одной строке. Маска не может содержать символа перевода строки
- Найденные вхождения не должны пересекаться.
- Пробелы и разделители участвуют в поиске наравне с другими символами.
- Можно использовать STL, Boost, возможности С++1x.
- Многопоточность нужно использовать обязательно. Однопоточные решения засчитываться не будут.
- Серьёзным плюсом будет разделение работы между потоками равномерно вне зависимости от количества строк во входном файле.

ПРИМЕР
Файл input.txt:
1 I've paid my dues <br>
2 Time after time. <br>
3 I've done my sentence <br>
4 But committed no crime. <br>
5 And bad mistakes ? <br>
6 I've made a few. <br>
7 I've had my share of sand kicked in my face <br>
8 But I've come through. <br>

Запуск программы: mtfind input.txt "?ad"
Ожидаемый результат:

3 <br>
5 5 bad <br>
6 6 mad <br>
7 6 had <br>

Решение представить в виде архива с исходным кодом и проектом CMake или Visual Studio (либо в виде ссылки на онлайн Git-репозиторий).
Код должен компилироваться в GCC или MSVC.

Критерии оценки решения:
1) Правильность выдаваемых результатов
2) Качество и читабельность кода, лёгкость дальнейшего развития и поддержки
3) Скорость работы и потребление памяти
</p>

<h2> Вызов программы </h2>
![image](https://user-images.githubusercontent.com/48221009/181612494-afce4e49-cd10-4ce9-af88-f7a3b5b72233.png)

<h2> Загрузка потоков ЦП при работе </h2>

