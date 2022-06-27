#include "diction.h"

#include <cstdio>
#include <fstream>
#include <algorithm>

Diction::Diction(std::string wTag, std::string tTag)
	:wTag(wTag), tTag(tTag)
{};

bool Diction::load(std::string file)
{
	std::ifstream stream(file, std::ios::binary);
	if (!stream.is_open()) {
		printf("[ERROR]Cann't open file: %s\n", file.c_str());
		exit(-1);
	}
	
	stream.seekg(0, std::ios::end);
	if (stream.tellg() > UINT32_MAX) {
		printf("[ERROR]File too big: %s\n", file.c_str());
		stream.close();
		exit(2);
	}
	this->_source_size = stream.tellg();
	stream.seekg(0, std::ios::beg);

	this->_source = std::make_unique<char[]>(this->_source_size);
	if (!this->_source) {
		printf("[ERROR]Out of memory!\n");
		stream.close();
		exit(-2);
	}
	
	stream.read(this->_source.get(), this->_source_size);
	if (!stream.good()) {
		printf("[ERROR]File is not good: %s\n", file.c_str());
		stream.close();
		exit(-1);
	}
	
	stream.close();
	return true;
}

bool Diction::parse()
{
	char* ptrStart = this->_source.get();
	char* ptrEnd = (char*)((size_t)ptrStart + this->_source_size);

	char* ptr = ptrStart;
	
	//UTF-8 BOM
	if (ptr[0] == 0xEF && ptr[1] == 0xBB && ptr[2] == 0xBF) {
		printf("UTF-8 with BOM\n");
		ptr += 3;
	}

	long lineCount = 1;
	bool inString = false;
	
	//Column Tags
	printf("Searching for word and translation tags...\n");
	int wCol = 0, tCol = 0, colCount = 0;
	for (std::string temp; ptr != ptrEnd ; ptr++) {
		//Next Line
		if (ptr[0] == '\r') {
			if (&ptr[1] != ptrEnd && ptr[1] == '\n')
			{
				colCount++;
				
				//Check Tag
				if (temp == this->wTag) {
					if (wCol > 0) {
						printf("[ERROR]Line %d:Redefined of word tag: %s\n", lineCount, this->wTag.c_str());
						exit(1);
					}
					wCol = colCount;
				}
				else if (temp == this->tTag) {
					if (tCol > 0) {
						printf("[ERROR]Line %d:Redefined of translation tag: %s\n", lineCount, this->tTag.c_str());
						exit(1);
					}
					tCol = colCount;
				}
				if (inString) {
					printf("[ERROR]Line %d:Quotation marks are not paired.\n", lineCount);
					exit(1);
				}
				ptr += 2;
				lineCount++;
				break;
			}
			printf("[ERROR]Line %d:Invalid end of line.\n", lineCount);
			exit(1);
		}
		if (ptr[0] == '\n') {
			printf("[ERROR]Line %d:Invalid end of line.\n", lineCount);
			exit(1);
		}
		
		//Quotation
		if (ptr[0] == '\"') {
			if (!(&ptr[1] != ptrEnd && ptr[1] == '\"')) {
				inString = !inString;
				continue;
			}
			temp.push_back('\"');
			ptr++;
			continue;
		}
		
		//Split
		if (ptr[0] == ',' && !inString) {
			colCount++;
			
			//Check Tag
			if (temp == this->wTag) {
				if (wCol > 0) {
					printf("[ERROR]Line %d:Redefined of word tag: %s\n", lineCount, this->wTag.c_str());
					exit(1);
				}
				wCol = colCount;
			}
			else if (temp == this->tTag) {
				if (tCol > 0) {
					printf("[ERROR]Line %d:Redefined of translation tag: %s\n", lineCount, this->tTag.c_str());
					exit(1);
				}
				tCol = colCount;
			}
			temp.clear();
			continue;
		}
		
		//Escape Character
		/*if (ptr[0] == '\\') {
			if (&ptr[1] != ptrEnd) {
				if (ptr[1] == 'n') {
					temp.push_back('\n');
					ptr++;
					continue;
				}
				else if (ptr[1] == 't') {
					temp.push_back('\t');
					ptr++;
					continue;
				}
			}
		}*/
		
		temp.push_back(ptr[0]);
	}
	
	if (colCount == 0) {
		printf("[ERROR]Line %d:Too short!\n", 1);
		exit(1);
	}
	if (wCol < 1) {
		printf("[ERROR]Line %d:Undefine of word tag: %s\n", 1, this->wTag.c_str());
		exit(1);
	}
	if (tCol < 1) {
		printf("[ERROR]Line %d:Undefine of translation tag: %s\n", 1, this->tTag.c_str());
		exit(1);
	}
	printf("Searching for word and translation tags...%d...%d\n", wCol, tCol);
	printf("Total column...%d\n", colCount);
	
	//Parse
	printf("Parsing...0\n");
	auto insertFunc = [this](char c, PlaceType type) {
		static PlaceType lastType = PlaceType::Other;
		if (type != lastType) {
			if (lastType == PlaceType::Word) {
				this->words.push_back('\n');
			}
			else if (lastType == PlaceType::Translate) {
				this->translations.push_back('\0');
			}
			
			if (type == PlaceType::Word) {
				this->wIndex.push_back(this->words.size());
			}
			else if (type == PlaceType::Translate) {
				this->tIndex.push_back(this->translations.size());
			}
				
			lastType = type;
		}
		
		if (type != PlaceType::Other) {
			if (c == '\0')
			{
				return;
			}
			if (type == PlaceType::Word) {
				this->words.push_back(c);
			}
			else if (type == PlaceType::Translate) {
				this->translations.push_back(c);
			}
		}
	};
	
	auto drawPercentFunc = [](int percent) {
		printf("Parsing...");
		for (int i = 0; i < percent; i++) {
			printf(".");
		}
		printf("%d\n", percent);
	};
	int percent = 0;
	
	inString = false;
	int colCurrent = 1;
	for (; ptr != ptrEnd; ptr++) {
		int per = ((size_t)ptr - (size_t)ptrStart) / (double)((size_t)ptrEnd - (size_t)ptrStart) * 100;
		if ((per - percent) >= (100 / 20.0)) {
			drawPercentFunc(per);
			percent = per;
		}
		
		//Next Line
		if (ptr[0] == '\r') {
			if (&ptr[1] != ptrEnd && ptr[1] == '\n')
			{
				if (colCurrent == wCol) {
					insertFunc('\0', PlaceType::Word);
				}
				if (colCurrent == tCol) {
					insertFunc('\0', PlaceType::Translate);
				}
				if (inString) {
					printf("[ERROR]Line %d:Quotation marks are not paired.\n", lineCount);
					exit(1);
				}
				if (colCurrent != colCount) {
					printf("[ERROR]Line %d:Number of columns does not match: %d/%d\n", lineCount, colCurrent, colCount);
					exit(1);
				}
				
				ptr++;
				if (ptr + 1 < ptrEnd) {
					colCurrent = 1;
				}
				lineCount++;
				continue;
			}
			printf("[ERROR]Line %d:Invalid end of line.\n", lineCount);
			exit(1);
		}
		if (ptr[0] == '\n') {
			printf("[ERROR]Line %d:Invalid end of line.\n", lineCount);
			exit(1);
		}
		
		//Quotation
		if (ptr[0] == '\"') {
			if (!(&ptr[1] != ptrEnd && ptr[1] == '\"')) {
				inString = !inString;
				continue;
			}
			if (colCurrent == wCol) {
				insertFunc('\"', PlaceType::Word);
			}
			if (colCurrent == tCol) {
				insertFunc('\"', PlaceType::Translate);
			}
			ptr++;
			continue;
		}

		//Split
		if (ptr[0] == ',' && !inString) {
			if (colCurrent == wCol) {
				insertFunc('\0', PlaceType::Word);
			}
			if (colCurrent == tCol) {
				insertFunc('\0', PlaceType::Translate);
			}
			colCurrent++;
			continue;
		}
		
		//Escape Character
		/*if (ptr[0] == '\\') {
			if (&ptr[1] != ptrEnd) {
				if (ptr[1] == 'n') {
					if (colCurrent == wCol) {
						printf("[ERROR]Line %d:Bad line breaks in words.\n", lineCount);
						exit(1);
					}
					if (colCurrent == tCol) {
						insertFunc('\n', PlaceType::Translate);
					}
					ptr++;
					continue;
				}
				else if (ptr[1] == 't') {
					if (colCurrent == wCol) {
						insertFunc('\t', PlaceType::Word);
					}
					if (colCurrent == tCol) {
						insertFunc('\t', PlaceType::Translate);
					}
					ptr++;
					continue;
				}
			}
		}*/
		
		if (colCurrent == wCol) {
			insertFunc(ptr[0], PlaceType::Word);
		}
		if (colCurrent == tCol) {
			insertFunc(ptr[0], PlaceType::Translate);
		}
	}
	if (inString) {
		printf("[ERROR]Line %d:Quotation marks are not paired.\n", lineCount);
		exit(1);
	}
	if (colCurrent != colCount) {
		printf("[ERROR]Line %d:Number of columns does not match: %d/%d\n", lineCount, colCurrent, colCount);
		exit(1);
	}
	
	insertFunc('\0', PlaceType::Other);
	this->words.push_back('\0');
	drawPercentFunc(100);
	
	if (this->wIndex.size() != this->tIndex.size()) {
		printf("[ERROR]The number of words and translations are not equal: %zd:%zd\n", this->wIndex.size(), this->tIndex.size());
		exit(1);
	}
	if (this->wIndex.size() == 0) {
		printf("[ERROR]No words!\n");
		exit(1);
	}
	
	printf("Total...%zd\n", this->wIndex.size());
	
	printf("Sorting...\n");
	this->quickSort(0, this->wIndex.size() - 1);
	printf("Sorting...Done\n");
	
	return true;
}

bool Diction::save(std::string file)
{
	std::ofstream stream(file, std::ios::binary | std::ios::trunc);
	if (!stream.is_open()) {
		printf("[ERROR]Cann't open file: %s\n", file.c_str());
		exit(-1);
	}

	uint32_t sizeIDW = (uint32_t)(this->wIndex.size() * sizeof(uint32_t));
	stream << this->secIdw;
	stream.write((char*)&sizeIDW, sizeof(uint32_t));
	stream.write((char*)this->wIndex.data(), sizeIDW);
	
	uint32_t sizeIDT = (uint32_t)(this->tIndex.size() * sizeof(uint32_t));
	stream << this->secIdt;
	stream.write((char*)&sizeIDT, sizeof(uint32_t));
	stream.write((char*)this->tIndex.data(), sizeIDT);
	
	uint32_t sizeLST = this->words.size();
	stream << this->secWords;
	stream.write((char*)&sizeLST, sizeof(uint32_t));
	stream.write(this->words.data(), sizeLST);
	
	uint32_t sizeTSL = this->translations.size();
	stream << this->secTrans;
	stream.write((char*)&sizeTSL, sizeof(uint32_t));
	stream.write(this->translations.data(), sizeTSL);
	
	if (!stream.good()) {
		printf("[ERROR]File is not good: %s\n", file.c_str());
		stream.close();
		exit(-1);
	}

	stream.close();
	return true;
}

//======

int Diction::strcmp(char* str1, char* str2)
{
	while ((*str1 != '\n') && (*str1 == *str2))
	{
		str1++;
		str2++;
	}
	return *str1 - *str2;
}

void Diction::quickSort(int left, int right)
{
	if (left >= right)return;
	
	int i = left, j = right;
	uint32_t base_w = this->wIndex[left];
	uint32_t base_t = this->tIndex[left];
	
	while (i < j) {
		while (Diction::strcmp((char*)((size_t)this->words.data() + this->wIndex[j]), (char*)((size_t)this->words.data() + base_w)) >= 0 &&
			i < j)j--;
		while (Diction::strcmp((char*)((size_t)this->words.data() + this->wIndex[i]), (char*)((size_t)this->words.data() + base_w)) <= 0 &&
			i < j)i++;
		if (i < j) {
			std::swap(this->wIndex[i], this->wIndex[j]);
			std::swap(this->tIndex[i], this->tIndex[j]);
		}
	}
	
	this->wIndex[left] = this->wIndex[i];
	this->tIndex[left] = this->tIndex[i];
	this->wIndex[i] = base_w;
	this->tIndex[i] = base_t;
	
	this->quickSort(left, i - 1);
	this->quickSort(i + 1, right);
}