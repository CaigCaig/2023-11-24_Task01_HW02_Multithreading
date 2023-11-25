#include <iostream>
#include <Windows.h>
#include <algorithm>
#include <execution>
#include <thread>
#include <random>
#include "timer.h"
#include <chrono>
#include <random>
#include <condition_variable>

using namespace std;
using namespace std::chrono_literals;
using namespace std::chrono;

// Параллельные вычисления
// Напишите программу для расчёта суммы двух векторов.
// Распараллельте эту программу на 2, 4, 8 и 16 потоков.
// Определите, какое количество потоков даёт самый быстрый результат.
// Сравните результаты выполнения для массивов из 1 000, 10 000, 100 000 и 1 000 000 элементов.
// Время выполнения для каждого варианта сведите в таблицу и выведите её в консоль.
// Первый запущенный поток должен вывести на экран доступное количество аппаратных ядер.

constexpr int vnums = 4;
constexpr int threads_num = 5;

vector<int> v1(1000000), v2(1000000), v3(1000000);
int i;
once_flag num_cores_f;
int vsize_num[vnums] = { 1000, 10'000, 100'000, 1'000'000 };

template <class Rep, class Period>
constexpr auto F(const std::chrono::duration<Rep, Period>& d)
{
	return std::chrono::duration<double>(d).count();
}

void prt_cores(void)
{
	int cores = thread::hardware_concurrency();
	cout << "Количество аппаратных ядер - " << cores << endl << endl;
	for (auto n = 0; n < vnums; ++n) cout << "\t\t" << vsize_num[n];
	cout << endl;
}

void sum(int vsize)
{
	call_once(num_cores_f, prt_cores);
	int x;
	while (i < vsize) {
		x = i++;
		if (i < vsize) v3[x] = v1[x] + v2[x];
	}
}

int main()
{
	setlocale(LC_ALL, "Russian");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	vector<thread>V;
	int threads[threads_num] = { 1, 2, 4, 8, 16 };
	double time[threads_num][vnums] = {0};
	mt19937 gen;
	uniform_int_distribution<int> dist(0, 255);
	auto rand_num([=]() mutable {return dist(gen); });
	generate(v1.begin(), v1.end(), rand_num);
	generate(v2.begin(), v2.end(), rand_num);

	for (auto t = 0; t < threads_num; ++t)
	{
		for (auto it = 0; it < vnums; ++it)
		{
			i = 0;
			for (int n = 0; n < threads[t]; ++n)
			{
				V.push_back(thread(sum, vsize_num[it]));
			}
			auto start = high_resolution_clock::now();
			for (auto& ths : V)
			{
				if (ths.joinable()) ths.join();
			}
			auto end = high_resolution_clock::now();
			time[t][it] = duration_cast<nanoseconds>(end - start).count() / 1'000'000'000.0;
		}
	}

	for (auto t = 0; t < threads_num; ++t)
	{
		cout << threads[t] << " потоков";
		for (auto it = 0; it < vnums; ++it)
		{
			cout << "\t" << time[t][it];
		}
		cout << endl;
	}

	system("pause");
}
