#define _CRT_SECURE_NO_WARNINGS 
#include <windows.h> 
#include <cstdlib> 
#include <iostream> 
#include <list> 
#include <string> 
#include <vector>

using namespace std;

typedef unsigned char byte;

struct WipeData
{
	int count;
	byte* data;
};
vector<WipeData> wipeData;

int COUNT = 0;
int k = 0;
int z = 0;
const byte b[27][3]
{
		{ 0x55, 0x55, 0x55 },
		{ 0xAA, 0xAA, 0xAA },
		{ 0x92, 0x49, 0x24 },
		{ 0x49, 0x24, 0x92 },
		{ 0x24, 0x92, 0x49 },
		{ 0x00, 0x00, 0x00 },
		{ 0x11, 0x11, 0x11 },
		{ 0x22, 0x22, 0x22 },
		{ 0x33, 0x33, 0x33 },
		{ 0x44, 0x44, 0x55 },
		{ 0x55, 0x55, 0x66 },
		{ 0x66, 0x66, 0x66 },
		{ 0x77, 0x77, 0x77 },
		{ 0x88, 0x88, 0x88 },
		{ 0x99, 0x99, 0x99 },
		{ 0xAA, 0xAA, 0xAA },
		{ 0xBB, 0xBB, 0xBB },
		{ 0xCC, 0xCC, 0xCC },
		{ 0xDD, 0xDD, 0xDD },
		{ 0xEE, 0xEE, 0xEE },
		{ 0xFF, 0xFF, 0xFF },
		{ 0x92, 0x49, 0x24 },
		{ 0x49, 0x24, 0x92 },
		{ 0x24, 0x92, 0x49 },
		{ 0x6D, 0xB6, 0xDB },
		{ 0xB6, 0xDB, 0x6D },
		{ 0xDB, 0x6D, 0xB6 },
};

int File_Find(WIN32_FIND_DATA* FindFile, string path, list<string>* data)
{
	HANDLE c;
	if (data == NULL)
	{
		cout << "Ошибка аргумента функции File_Find" << endl;
		system("pause");
		return 13;
	}
	if (FindFile == NULL)
	{
		cout << "Ошибка аргумента функции File_Find" << endl;
		system("pause");
		return 11;
	}
	c = FindFirstFile((path + "\\*").c_str(), FindFile);
	if (c == INVALID_HANDLE_VALUE)
	{
		c = FindFirstFile(path.c_str(), FindFile);
		if (c == INVALID_HANDLE_VALUE)
		{
			cout << "Некорректный путь или указанная директория отсутствует\n ";
			system("pause");
			return 3;
		}
		if (!(FindFile->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			k++;
			data->push_front(path);
			return 0;
		}
	}
	else
	{
		do
		{
			if (lstrcmpi(FindFile->cFileName, ".") == 0 || lstrcmpi(FindFile->cFileName, "..") == 0)
				continue;
			if (!(FindFile->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				data->push_front(path + "\\" + FindFile->cFileName);
				k++;
			}
			else
				if (FindFile->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					data->push_front(path + "\\" + FindFile->cFileName);
					z++;
					File_Find(FindFile, path + "\\" + FindFile->cFileName, data);
				}
		} while (FindNextFile(c, FindFile));
	}
	if (FindClose(c) == 0)
	{
		cout << "Ошибка закрытия поиска файла!" << endl;
		system("pause");
		return 1;
	}
	return 0;
}

int IsItFile(WIN32_FIND_DATA* FindFile, string path)
{
	HANDLE c;
	if (FindFile == NULL)
	{
		cout << "Ошибка функции File_Find" << endl;
		system("pause");
		return -1;
	}
	c = FindFirstFile(path.c_str(), FindFile);
	if (c == INVALID_HANDLE_VALUE)
	{
		cout << "Некорректный путь или указанная директория отсутствует\n ";
		system("pause");
		return 3;
	}
	if (!(FindFile->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		return 1;
	else
		return 0;
}

int Rewrite(string path, const byte* value, int FileSize)
{
	FILE* f = fopen(path.c_str(), "r+b");
	if (f == NULL)
	{
		cout << "Невозможно открыть файл";
		system("pause");
		return 2;
	}
	if (FileSize < 0)
	{
		cout << "Ошибка аргумента функции Rewrite" << endl;
		system("pause");
		return 13;
	}
	for (int i = 0; i < FileSize; i++)
		if (fwrite((char*)&value[i % 3], sizeof(value[i % 3]), 1, f) < 1)
		{
			cout << "Ошибка выполнения функции fwrite" << endl;
			system("pause");
			return -3;
		}
	if (fclose(f) == EOF)
	{
		cout << "Не удалось закрыть файл" << endl;
		system("pause");
		return 1;
	}
	return 0;
}

int	RewriteFileRnd(string path, int FileSize)
{
	FILE* f = fopen(path.c_str(), "r+b");
	byte* rndbytes;
	if (f == NULL)
	{
		cout << "Невозможно открыть файл";
		system("pause");
		return 2;
	}
	if (FileSize < 0)
	{
		cout << "Ошибка аргумента функции Rewrite" << endl;
		system("pause");
		return 12;
	}
	try
	{
		rndbytes = new byte[FileSize];
		if (fseek(f, 0, 0) != 0)
		{
			cout << "Ошибка функции fseek" << endl;
			system("pause");
			return -2;
		}
		HCRYPTPROV hCryptCtx = NULL;
		if (CryptAcquireContext(&hCryptCtx, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) == 0)
		{
			cout << "Ошибка функции CryptAcquireContext " << endl;
			system("pause");
			return -5;
		}
		if (CryptGenRandom(hCryptCtx, FileSize, rndbytes) == 0)
		{
			cout << "Ошибка функции CryptGenRandom " << endl;
			system("pause");
			return -5;
		}
		if (CryptReleaseContext(hCryptCtx, 0) == 0)
		{
			cout << "Ошибка функции CryptReleaseContext " << endl;
			system("pause");
			return -5;
		}

		if (fwrite(rndbytes, sizeof(rndbytes[0]), FileSize, f) < FileSize)
		{
			cout << "Ошибка выполнения функции fwrite" << endl;
			system("pause");
			return -3;
		}
	}
	catch (bad_alloc ex)
	{
		cout << "Ошибка выделения памяти!" << endl;
		system("pause");
		return 5;
	}


	memset(rndbytes, 0xCC, FileSize);
	delete[] rndbytes;

	if (fclose(f) == EOF)
	{
		cout << "Не удалось закрыть файл" << endl;
		system("pause");
		return 1;
	}
	return 0;
}

int GetBytes()
{
	do
	{
		cout << "Введите сколько раз хотите перезаписать файлы" << endl;
		cin >> COUNT;
	} while (COUNT <= 0);

	for (int i = 0; i < COUNT; i++)
	{
		WipeData data;
		cout << "Введите количество байтов для " << i + 1 << " - раза" << endl;
		cin >> data.count;
		try
		{
			data.data = new byte[data.count];
		}
		catch (bad_alloc ex)
		{
			cout << "Ошибка выделения памяти!" << endl;
			system("pause");
			return 5;
		}
		cout << "Введите байты" << endl;
		for (int j = 0; j < data.count; j++)
		{
			unsigned buff = 0;
			cin >> buff;
			data.data[j] = buff;
		}
		wipeData.push_back(data);
	}
	return 0;
}

void DeleteMyBytes()
{
	for (int i = 0; i < COUNT; i++)
	{
		memset(wipeData[i].data, 0xCC, wipeData[i].count);
		delete[] wipeData[i].data;
		wipeData[i].count = 0;
	}
}

int RewriteUsingMyBytes(string path, int FileSize)
{
	FILE* f = fopen(path.c_str(), "r+b");
	if (f == NULL)
	{
		cout << "Невозможно открыть файл" << endl;
		system("pause");
		return 2;
	}
	if (FileSize < 0)
	{
		cout << "Ошибка аргумента функции Rewrite" << endl;
		system("pause");
		return 12;
	}
	for (int i = 0; i < COUNT; i++)
	{
		if (fseek(f, 0, 0) != 0)
		{
			cout << "Ошибка функции fseek" << endl;
			system("pause");
			return -2;
		}
		for (int z = 0; z < FileSize; z++)
			if (fwrite((char*)&(wipeData[i].data[z % (wipeData[i].count)]), sizeof(wipeData[i].data[z % (wipeData[i].count)]), 1, f) < 1)
			{
				cout << "Ошибка выполнения функции fwrite" << endl;
				system("pause");
				return -3;
			}
	}
	if (fclose(f) == EOF)
	{
		cout << "Не удалось закрыть файл" << endl;
		system("pause");
		return 1;
	}
	return 0;
}

int FileGutmanRewrite(string path, int FileSize)
{
	if (FileSize < 0)
	{
		cout << "Ошибка аргумента функции FileGutmanRewrite" << endl;
		system("pause");
		return 12;
	}
	for (int i = 0; i < 4; i++)
		if (RewriteFileRnd(path, FileSize) != 0)
		{
			cout << "Ошибка выполнения функции RewriteFileRnd" << endl;
			system("pause");
			return 6;
		}
	for (int i = 0; i < 27; i++)
		if (Rewrite(path, b[i], FileSize) != 0)
		{
			cout << "Ошибка выполнения функции Rewrite" << endl;
			system("pause");
			return -6;
		}
	for (int i = 0; i < 4; i++)
		if (RewriteFileRnd(path, FileSize) != 0)
		{
			cout << "Ошибка аргумента функции RewriteFileRnd" << endl;
			system("pause");
			return 6;
		}
	return 0;
}

int DeleteAllFiles(string path, list<string>* data, int Choise)
{
	int FileSize;
	list<string>::iterator i;
	if (data == NULL)
	{
		cout << "Ошибка аргумента функции DeleteAllFiles" << endl;
		system("pause");
		return 12;
	}
	if (Choise != 1 && Choise != 2)
	{
		cout << "Ошибка аргумента функции DeleteAllFiles" << endl;
		system("pause");
		return 13;
	}
	for (i = data->begin(); i != data->end(); ++i)
	{
		DWORD dwAttr = GetFileAttributes((*i).c_str());
		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (RemoveDirectory((*i).c_str()))
				cout << "Удалим папку " << *i << endl;
			else
				cout << "Не удалось удалить папку [" << *i << "]. Удалите ее вручную" << endl;
		}
		else
		{
			FILE* f = fopen((*i).c_str(), "rb");
			if (f == NULL)
			{
				cout << "Невозможно открыть файл";
				return ERROR;
			}
			if (fseek(f, 0, SEEK_END) != 0)
			{
				cout << "Ошибка функции fseek" << endl;
				system("pause");
				return -2;
			}
			FileSize = ftell(f);
			if (ftell(f) == -1L)
			{
				cout << "Ошибка функции ftell" << endl;
				system("pause");
				return -1;
			}
			if (fclose(f) == EOF)
			{
				cout << "Не удалось закрыть файл" << endl;
				system("pause");
				return 1;
			}
			cout << "Удалим " << *i << " Размера " << FileSize << " байт" << endl;
			if (Choise == 1)
				if (FileGutmanRewrite((*i).c_str(), FileSize) != 0)
				{
					cout << "Ошибка функции FileGutmanRewrite" << endl;
					system("pause");
					return 7;
				}
				else
					if (Choise == 2)
						if (RewriteUsingMyBytes((*i).c_str(), FileSize) != 0)
						{
							cout << "Ошибка функции RewriteUsingMyBytes" << endl;
							system("pause");
							return -7;
						}
			if (remove((*i).c_str()) != 0)
			{
				cout << "Не удалось удалить файл " << *i << endl;
			}
		}
	}
	return 0;
}


int main()
{
	setlocale(LC_ALL, "RUS");
	WIN32_FIND_DATA FindFile;
	list <string> data;
	list<string>::iterator i;
	char Confirm = 'N';
	int Choise = 0;
	string path;
	cout << "Безопасное удаление файлов\n";
	cout << "Курсовую выполнил Гойколов Илья\n";
	cout << "Введите путь к файлу, который нужно удалить. (Например: C:\\Games\\CS GO)" << endl;
	cin >> path;
	if (IsItFile(&FindFile, path) != 1 && IsItFile(&FindFile, path) != 0)
	{
		cout << "Ошибка выполнения функции IsItFile" << endl;
		system("pause");
		return 0;
	}
	else
		if (IsItFile(&FindFile, path) == 0)
		{
			data.push_front(path);
			z++;
		}

	if (File_Find(&FindFile, path, &data) != 0)
	{
		cout << "Ошибка выполнения функции File_Find" << endl;
		system("pause");
		return 0;
	}

	cout << endl;
	cout << "------------------------------------------------- " << endl;
	cout << "Список файлов и папок, которые будут удалены: " << endl;
	cout << "------------------------------------------------- " << endl;
	for (i = data.begin(); i != data.end(); i++)
	{
		cout << *i << endl;
	}
	cout << "\nКоличество файлов = " << k << endl;
	cout << "Количество папок = " << z << endl;

	while (Confirm != 'Y')
	{
		cout << "Вы точно хотите удалить все файлы по указанному пути? \nДействие необратимо! \nВВедите да или нет соответственно - Y\\N ?\n" << endl;
		cin >> Confirm;
		if (Confirm == 'N')
		{
			cout << "Спасибо за использование моей программы!" << endl;
			system("pause");
			return 0;
		}
		else
			if (Confirm != 'Y')
			{
				cout << "Введите один из символов: Y или N" << endl;
			}
	}
	while (Choise != 1 || Choise != 2)
	{
		cout << "Ввыберите способ удаления файлов" << endl;
		cout << "1) Использовать алгоритм Гутмана" << endl;
		cout << "2) Ввести байты для перезаписи" << endl;
		cin >> Choise;
		if (Choise == 1)
		{
			if (DeleteAllFiles(path, &data, Choise) != 0)
			{
				cout << "Ошибка выполнения функции DeleteAllFiles" << endl;
				system("pause");
				return 0;
			}
			break;
		}
		else
			if (Choise == 2)
			{
				if (GetBytes() != 0)
				{
					cout << "Ошибка выполнения функции GetBytes" << endl;
					system("pause");
					return 0;
				}
				if (DeleteAllFiles(path, &data, Choise) != 0)
				{
					cout << "Ошибка выполнения функции DeleteAllFiles" << endl;
					system("pause");
					return 0;
				}
				DeleteMyBytes();
				break;
			}
			else
				cout << "Выберите один из предложенных вариантов" << endl;
	}
	cout << "Спасибо за использование моей программы!" << endl;
	system("pause");
	return 0;
}
