#include <iostream>
#include <dirent.h>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <climits>
#include <iomanip>

using namespace std;

vector < pair<string, int> > merge(vector < pair<string, int> > l, vector < pair<string, int> > r)
{
	vector < pair<string, int> > result;
	while (l.size() > 0 || r.size() > 0)
	{
		if (l.size() > 0 && r. size() > 0)
		{
			if (l.begin()->second > r.begin()->second)
			{
				result.push_back(l.front());
				l.erase(l.begin());
			}
			else if (l.begin()->second == r.begin()->second)
			{
				if ((l.begin()->first).compare(r.begin()->first) < 0)
				{
					result.push_back(l.front());
					l.erase(l.begin());
				}
				else
				{
					result.push_back(r.front());
					r.erase(r.begin());
				}
			}
			else
			{
				result.push_back(r.front());
				r.erase(r.begin());
			}
		}
		else if (l.size() > 0)
		{
			result.push_back(l.front());
			l.erase(l.begin());
		}
		else if (r.size() > 0)
		{
			result.push_back(r.front());
			r.erase(r.begin());
		}
	}
	return result;
}

void MSort(vector < pair<string, int> > &topFreq)
{
	int size = topFreq.size();
	if (size <= 1) return;

	vector < pair<string, int> > l;
	vector < pair<string, int> > r;
	int m = size / 2;
	for (int i = 0; i < m; ++i)
	{
		l.push_back(topFreq[i]);
	}
	for (int i = m; i < size; ++i)
	{
		r.push_back(topFreq[i]);
	}

	MSort(l);
	MSort(r);

    topFreq = merge(l, r);
}

void trim(char* &inLine)
{
	int i = strlen(inLine)-1;
	if ((inLine[i] == 't' || inLine[i] == 'T') && (inLine[i-1] == 'x' || inLine[i-1] == 'X') && (inLine[i-2] == 't' || inLine[i-2] == 'T') && (inLine[i-3] == '.'))
		inLine[i-3] = '\0';
}

int isdirectory(char *path)
{
	struct stat statbuf;

	if (stat(path, &statbuf) == -1)
	{
		perror("while calling stat()");
		return -1;
	}
	else
	{
		return S_ISDIR(statbuf.st_mode);
	}
}

int getFolderSize(char *path)
{
	int size = 0;
	DIR *dir;
	struct dirent *ent;
	dir = opendir(path);
	if (dir != NULL)
	{
		// get rid of the first two lines with dots
		int i = 0;
		while (i++ < 2)
		{
			readdir(dir);
		}


		while ((ent = readdir(dir)) != NULL)
		{
			char* inLine = ent->d_name;
			char tempPath[1000];
			strcpy(tempPath, path);
			strcat(tempPath, "/");
			strcat(tempPath, inLine);
			//cout << inLine << endl;
			//cout << path << endl;
			//cout << tempPath << endl;

			struct stat filestatus;
			stat(tempPath, &filestatus);
			
			if (isdirectory(tempPath))
			{
				size += getFolderSize(tempPath);
				//cout << "Size1: " << size << endl;
			}
			else
			{
				size += (int)filestatus.st_size;
				//cout << "Size2: " << size << endl;
			}
		}
	}
	else
	{
		perror("");
		return EXIT_FAILURE;
	}
	closedir(dir);

	return size;
}

int getList(vector< pair<string, int> > &list, char *path, long long &totalSize)
{
	int size = 0;
	DIR *dir;
	struct dirent *ent;
	dir = opendir(path);
	if (dir != NULL)
	{
		// get rid of the first two lines with dots
		int i = 0;
		while (i++ < 2)
		{
			readdir(dir);
		}

		// store 《
		string t;
		t = "《";
		//cout << t << endl;

		// normal read-in, start from 3rd line
		char* inLine;
		while ((ent = readdir(dir)) != NULL)
		{
			inLine = ent->d_name;

			// Get first charactor
			char *f = new char[4];
			for (int i = 0; i < 3; ++i)
			{
				f[i] = inLine[i];
			}
			f[3] = '\0';

			char tempPath[1000];
			strcpy(tempPath, path);
			strcat(tempPath, inLine);
			//cout << tempPath << endl;
			struct stat filestatus;
			stat(tempPath, &filestatus);

			// compare with 《. If no 《, skip putting it into vector, but traverse its content.
			if (strcmp(t.c_str(), f) != 0)
			{
				strcat(tempPath, "/");
				getList(list, tempPath, totalSize);
				continue;
			}
			delete[] f;
			if (isdirectory(tempPath))
			{
				size = (int)getFolderSize(tempPath);
				list.push_back(make_pair(inLine, size));
				//printf("Size of %s is: %d\n", inLine, size);
			}
			else
			{
				trim(inLine);
				size = (int)filestatus.st_size;
				list.push_back(make_pair(inLine, size));
			}
			totalSize += size;
			//cout << "current totalSize: " << totalSize << endl;
		}
		closedir(dir);
		return 0;
	}
	else
	{
		perror("");
		return EXIT_FAILURE;
	}
}

int main()
{
	vector< pair<string, int> > list;

	ofstream outputFile;
	outputFile.open("/cygdrive/E/Book/TXT/小说/精校目录.txt");

	char path[1000] = "/cygdrive/E/Book/TXT/小说/精校/";

	long long totalSize = 0;

	// get list vector
	getList(list, path, totalSize);
	long long mid = totalSize / 2;
	// sort vector
	MSort(list);

	// output only the names
	vector < pair<string, int> >::const_iterator it, m;
	int min = INT_MAX;
	long long tempSize = totalSize;
	int num = 0;
	int rank = 0;
	for (it = list.begin(); it != list.end(); it++, num++)
	{
		tempSize -= it->second;
		int temp = abs(int(tempSize - mid));
		if (temp < min)
		{
			min = temp;
			m = it;
			rank = num + 1;
		}
		//printf("abs(%d - %d) = %d\n", it->second, mid, temp);
		outputFile << it->first << endl;
	}
	//cout << "totalSize: " << totalSize << endl;
	outputFile << "\n\n\n精校书籍数量： " << num << " 本\n" << "精校书籍大小： " << setprecision(3) << float(totalSize)/1024.00f/1024.00f/1024.00f << " GB\n\n" 
				<< "中位书籍名称： " << m->first << endl << "中位书籍大小： " << float(m->second)/1024.00f/1024.00f << " MB\n" << "中位书籍排名： " << rank << " / " << num;
	outputFile.close();
}