// File: Dataset.h / cpp
// Purpose: dataset
// Author: Pavel Skribtsov
// Date: 25-12-08
// Version 1.0
// (C) PAWLIN TECHNOLOGIES LTD. ALL RIGHTS RESERVED


#include "stdafx.h"

#include <PWNGeneral/Dataset.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory.h>

Dataset::Dataset (const Dataset &other, const std::vector<bool> &columnsMask) {
	this->rows.resize(other.rows.size());
	size_t columns = 0, columns2copy = 0;
	size_t othercolumns = other.columns();  
	for(size_t n = 0; n < columnsMask.size(); n++){ //other.columns(); n++)
		if(columnsMask[n]){
			columns ++;
			if(n < othercolumns)
				columns2copy++;
		}
	}      
	if(othercolumns > columnsMask.size())
		othercolumns =   columnsMask.size();
	for(size_t i = 0; i < rows.size(); i++) {
		//rows[i].data.reserve(columns);
		if(columns2copy ==  columns)
			 rows[i].data.resize(columns);
		else rows[i].data.resize(columns,0);
		//unsigned k = 0; 
		float * ptr_dst =  &rows[i].data[0];
		const float * ptr_src =  &other.rows[i].data[0];
		if(columns2copy ==  othercolumns)
			memcpy(ptr_dst,ptr_src,sizeof(float)*columns);
		else 		
		for(unsigned j = 0; j < othercolumns; j++) //other.columns(); j++) {
			if(columnsMask[j]) 
				 *ptr_dst++ =  *ptr_src++;
			else ptr_src++;
			//rows[i].data[k++] = other.rows[i].data[j];
			//rows[i].data.push_back(other.rows[i].data[j]);				  
	}	
}


void Dataset::makeVariants(std::vector <std::map<float, size_t> *>& values) const
{
	values.resize(columns(),NULL);
	// enumerate all values variants
	FOR_ALL(values, i) values[i] = new std::map<float, size_t>();
	FOR_ALL(rows, q) {
		FOR_ALL(rows[q].data, i) {
			float v = rows[q].data[i];
			values[i]->operator[](v) = 0;
		}
	}
	// assign each variant it's order number
	FOR_ALL(values, i) {
		size_t count = 0;
		for (auto iter = values[i]->begin(); iter != values[i]->end(); iter++, count++) {
			iter->second = count;
		}
	}
}

void Dataset::makeVariants(std::vector <std::map<float, size_t> >& values) const
{
	values.resize(columns());
	// enumerate all values variants
	FOR_ALL(rows, q) {
		FOR_ALL(rows[q].data, i) {
			float v = rows[q].data[i];
			values[i][v] = 0;
		}
	}
	// assign each variant it's order number
	FOR_ALL(values, i) {
		size_t count = 0;
		for (auto iter = values[i].begin(); iter != values[i].end(); iter++, count++) {
			iter->second = count;
		}
	}
}

void Dataset::makeVariants(std::vector<std::set<float>>& values) const
{
	values.resize(columns());
	// enumerate all values variants
	FOR_ALL(rows, q) {
		FOR_ALL(rows[q].data, i) {
			float v = rows[q].data[i];
			values[i].insert(v);
		}
	}
}

void Dataset::computeMinMaxAvg(
							   FloatVector &setmin, 
							   FloatVector &setmax, 
							   FloatVector &setavg,
								size_t startIndex, 
								size_t rowCount,
								int maxcolumns 
							   ) const {
	size_t size =  rows[startIndex].data.size();
	if(maxcolumns > 0) size = std::min<int>((int)size, maxcolumns);
	setmin.data.resize(size);
	setmax.data.resize(size);
	setavg.data.resize(size,0);
	for(size_t i = 0; i < size; i++) setmin[i] = setmax[i] = rows[startIndex].data[i];
	size_t to = startIndex + rowCount;
	for(size_t n = startIndex; n < to; n++) {
		for(size_t i = 0; i < size; i++) {
			float v = rows[n].data[i];
			if (!finite_check(v)) {
				printf("Warning at row %zu, col %zu - infinite value\n", n, i);
			}
			setmin[i] = v < setmin[i] ? v : setmin[i];
			setmax[i] = v > setmax[i] ? v : setmax[i];
			setavg[i] += v;
		}
	}
	for(size_t i = 0; i < size; i++) setavg[i] /= (float)rowCount;
};

void Dataset:: computeMinMaxAvg(FloatVector &setmin, FloatVector &setmax, FloatVector &setavg,	
		const Dataset& cut_off_min_max,
		size_t startIndex, 
		size_t rowCount, int maxcolumns) const {


	size_t size =  rows[startIndex].data.size();
	if(maxcolumns > 0) size = std::min<int>((int)size, maxcolumns);
	setmin.data.resize(size);
	setmax.data.resize(size);
	setavg.data.resize(size,0);
	for(size_t i = 0; i < size; i++) setmin[i] = setmax[i] = rows[startIndex].data[i];
	size_t to = startIndex + rowCount;
	std::vector <int> counts(size, 0);
	for(size_t n = startIndex; n < to; n++) {
		

		for(size_t i = 0; i < size; i++) {
			float v = rows[n].data[i];	

			float cut_off_min_val = cut_off_min_max.rows[0].data[i];
			float cut_off_max_val = cut_off_min_max.rows[1].data[i];

			if(v < cut_off_min_val) {
				continue;
			}

			if(v > cut_off_max_val) {
				continue;
			}

			setmin[i] = v < setmin[i] ? v : setmin[i];
			setmax[i] = v > setmax[i] ? v : setmax[i];
			counts[i]++;
			setavg[i] += v;
		}
		
	}
	for(size_t i = 0; i < size; i++) setavg[i] /= float(counts[i]);

}

void Dataset::normalize(const DatasetSimpleNormalization &norm) {
	for(size_t i = 0; i < rows.size(); i++) norm.normalize(rows[i]);
}

void Dataset::denormalize(const DatasetSimpleNormalization &norm) {
	for (size_t i = 0; i < rows.size(); i++) norm.denormalize(rows[i]);
}

const char *SimpleNormalization::fileheader = "PWNLIB 1.0 normalization (averages,scales)";
const char *BooleanMask::fileheader = "PWNLIB 1.0 features mask";

void DatasetSimpleNormalization::init(
	const Dataset &dataset, 
	size_t startIndex, 
	size_t rowCount, 
	bool doubleRange,// by default (true) normalization maps to range [-1 ; 1], otherwise [-0.5; 0.5]
	int maxcolumns) 
{
	FloatVector setmin; 
	FloatVector setmax;
	dataset.computeMinMaxAvg(setmin,setmax,avg, startIndex, rowCount, maxcolumns);
	size_t size = dataset.rows[startIndex].data.size();
	if(maxcolumns > 0) size = std::min<int>((int)size, maxcolumns);
	this->scales.data.resize(size);
	for(size_t i = 0; i < size; i++) {
		float range = (setmax[i] - setmin[i]);
		if(range == 0) range = 1.0f; // no meaningful scale possible
		scales[i] = (doubleRange ? 2.0f : 1.0f) / range ;
	}
}

DatasetSimpleNormalization::DatasetSimpleNormalization(const Dataset &dataset, 
													   size_t startIndex, 
													   size_t rowCount, 
													   FloatVector &setmin,
													   FloatVector &setmax,
													   bool doubleRange,
													   int maxcolumns) {// by default (true) normalization maps to range [-1 ; 1], otherwise [-0.5; 0.5]
	dataset.computeMinMaxAvg(setmin,setmax,avg, startIndex, rowCount);
	size_t size = dataset.rows[startIndex].data.size();
	if(maxcolumns > 0) size = std::min<int>((int)size, maxcolumns);
	this->scales.data.resize(size);
	for(size_t i = 0; i < size; i++) {
		float range = (setmax[i] - setmin[i]);
		if(range == 0) range = 1.0f; // no meaningful scale possible
		scales[i] = (doubleRange ? 2.0f : 1.0f) / range ;
	}
}

SimpleMatrixSimpleNormalization::SimpleMatrixSimpleNormalization(const SimpleMatrix &matr,
																 size_t startIndex, 
																 size_t rowCount,
																 bool doubleRange,
																 int maxcolumns){
	FloatVector setmin; 
	FloatVector setmax;
	computeMatrixMinMaxAverage(matr,setmin,setmax,avg, startIndex, rowCount, maxcolumns);
	size_t size = matr.getCols();
	if(maxcolumns > 0) size = std::min<int>((int)size, maxcolumns);
	this->scales.data.resize(size);
	/*float setmax_max = -FLT_MAX;
	float setmax_min = FLT_MAX;	
	for(size_t i = 0; i < size; i++) {
		 if(setmax[i] > setmax_max) {
			setmax_max = setmax[i];
		 }

		 if(setmin[i] < setmax_min) {
			setmax_min = setmin[i];
		 }			
	}*/
	
	//float range = (setmax_max - setmax_min);
	for(unsigned i = 0; i < size; i++) {
		float range = (setmax[i] - setmin[i]);
		if(range < FLT_EPSILON) range = 1.0f; // no meaningful scale possible
		scales[i] = (doubleRange ? 2.0f : 1.0f) / range ;
	}

	
}
/*SimpleMatrixSimpleNormalization::SimpleMatrixSimpleNormalization(const SimpleMatrix &matrix, 
																 size_t startIndex, 
																 size_t rowCount, 
																 FloatVector &setmin,
																 FloatVector &setmax, 
																 bool doubleRange ,
																 int maxcolumns){


}*/
void SimpleMatrixSimpleNormalization::computeMatrixMinMaxAverage(const SimpleMatrix &matr,
															FloatVector &setmin, 
															FloatVector &setmax, 
															FloatVector &setavg,
															size_t startIndex, 
															size_t rowCount,
															int maxcolumns )const {
													
	size_t size = matr.getCols();
	if(maxcolumns > 0) size = std::min<int>((int)size, maxcolumns);
	setmin.data.resize(size);
	setmax.data.resize(size);
	setavg.data.resize(size,0);
	for(size_t i = 0; i < size; i++) setmin[i] = setmax[i] = matr[startIndex][i];
	size_t to = startIndex + rowCount;
	for(size_t n = startIndex; n < to; n++) {
		for(size_t i = 0; i < size; i++) {
			float v = matr[n][i];
			setmin[i] = v < setmin[i] ? v : setmin[i];
			setmax[i] = v > setmax[i] ? v : setmax[i];
			setavg[i] += v;
		}
	}
	for(size_t i = 0; i < size; i++) setavg[i] /= (float)rowCount;

	////---
	//size_t size = matr.getCols();
	//if(maxcolumns > 0) size = std::min<int>((int)size, maxcolumns);
	//setmin.data.clear();
	//setmin.data.resize(size, 0.0f);
	//setmax.data.clear();
	//setmax.data.resize(size, 0.0f);
	//setavg.data.clear();
	//setavg.data.resize(size, 0.0f);
	//
	//size_t to = startIndex + rowCount;
	//for(size_t n = startIndex; n < to; n++) {
	//	for(size_t i = 0; i < size; i++) {
	//		float v = matr[n][i];
	//		setmax[i] += v*v;
	//		setavg[i] += v;
	//	}
	//}
	//for(size_t i = 0; i < size; i++) { 
	//	setmax[i] /= (float)rowCount; 
	//	setavg[i] /= (float)rowCount; 	
	//}
	//for(size_t i = 0; i < size; i++) { 
	//	setmax[i] -= setavg[i]*setavg[i];
	//	if (setmax[i] < 0.0f) setmax[i] = 0.0f;		
	//}
	//float v = 4.0f*sqrtf(setmax.components_sum() / float(size));
	//setmax.data.clear();
	//setmax.data.resize(size, v);
	////---





}



void SimpleNormalization::normalize(Dataset &sequence) const {
	size_t size = sequence.rows.size();
	for(size_t i = 0; i < size; i++) normalize(sequence.rows[i]);
}
void SimpleNormalization::normalize(Dataset &sequence, size_t dims) const {
	size_t size = sequence.rows.size();
	for(size_t i = 0; i < size; i++) normalize(sequence.rows[i],dims);
}

bool Dataset::saveCSV(FILE* file, bool useComma) const {
	if(!file) {
		return false;
	}
	for(size_t i = 0; i < rows.size(); i++) {
		int lastIndex = int(rows[i].data.size()-1);
		if(lastIndex < 0) {
			continue; //don't write empty rows
		}
		for(size_t j = 0; j < size_t(lastIndex); j++)  {
			if(useComma) {
				fprintf(file,"%f,", rows[i][j]);
			}
			else {
				fprintf(file,"%f;", rows[i][j]);
			}
		}
		fprintf(file,"%f\n", rows[i][lastIndex]);

	}
	return true;
}

bool Dataset::saveCSVmode(const char *filename, const std::string &mode, bool useComma, const vector<string>&header) const {
	FILE *file = fopen(filename, mode.data());
	if(!file) return false;	
	const char *sep = useComma ? "," : ";";
	if (!header.empty()) 
		FOR_ALL(header, h) 
			fprintf(file, "%s%s", header[h].c_str(), h == header.size() - 1 ? "\n" : sep);
	bool res = saveCSV(file,useComma);	
	fclose(file);
	return res;
}


bool Dataset::saveBIN(FILE *file) const {
	
	//try {
	uint64_t nRows = 0;
		uint64_t nCols = columns();
		for (size_t i = 0; i< rows.size(); i++)
			if (rows[i].data.size()== nCols) nRows++;
		const char* header = "(C) Pawlin Technologies Ltd. Binary Spreadsheets file format 2.0";
		fwrite(header, sizeof(char), strlen(header), file);
		fwrite(&nRows, sizeof(uint64_t), 1, file);
		fwrite(&nCols, sizeof(uint64_t), 1, file);
		for(unsigned i = 0; i < rows.size(); i++) {
			if (rows[i].data.size() == nCols)
				fwrite(&rows[i].data[0], sizeof(float), (size_t) nCols, file);
		}
	//}
	//catch(const char* err){
	//	return false;
	//}

	return true;
}
bool Dataset::loadBIN(FILE *file){

	//try {
		uint64_t nRows = 0;
		uint64_t nCols = 0;
		

		const char* header = "(C) Pawlin Technologies Ltd. Binary Spreadsheets file format 2.0";
		size_t bufsize = strlen(header);
		char* buf = new char[bufsize+1];
		buf[bufsize]=0;
		fread(buf, sizeof(char), bufsize, file);
		if(strcmp(header,buf)){
			//printf("buf    = %s|%d\nheader = %s|%d bufsize = %d\n", buf, strlen(buf), header, strlen(header), bufsize);
			throw("incompatible binary dataset format");
		}
		delete [] buf;
		fread(&nRows, sizeof(uint64_t), 1, file);
		fread(&nCols, sizeof(uint64_t), 1, file);
		
		rows.resize((size_t) nRows);
		for (size_t i = 0; i < rows.size(); i++) {
			rows[i].resize((size_t) nCols);
		}	

		for(size_t i = 0; i < rows.size(); i++) {
			fread(&rows[i].data[0], sizeof(float), (size_t) nCols, file);
		}
	//}
	//catch(const char* err){
	//	return false;
	//}

	return true;

}


bool Dataset::saveBIN(const char *filename) const {
	/*FILE *file = fopen(filename,"wb");
	if(!file) return false;
	unsigned int nRows = 0;
	unsigned int nCols = columns();
	for (unsigned int i = 0; i< rows.size(); i++)
		if (rows[i].data.size()== nCols) nRows++;

	const char* header = "(C) Pawlin Technologies Ltd. Binary Spreadsheets file format.\n";
	fwrite(header, sizeof(char), strlen(header), file);
	fwrite(&nRows, sizeof(unsigned int), 1, file);
	fwrite(&nCols, sizeof(unsigned int), 1, file);
	for(unsigned i = 0; i < rows.size(); i++) {

		if (rows[i].data.size() == nCols)
			fwrite(&rows[i].data[0], sizeof(float), nCols, file);
	}
	fclose(file);
	
	return true;*/
	FILE *file = fopen(filename,"wb");
	if (!file) return false;
	bool res = saveBIN(file);
	fclose(file);
	return res;

}

bool Dataset::loadBIN(const char *filename)  {
	
	FILE *file = fopen(filename,"rb");
	if(!file) return false;
	bool res = loadBIN(file);
	fclose(file);
	
	return res;


	/*FILE *file = fopen(filename,"rb");
	if(!file) return false;
	unsigned int nRows = 0;
	unsigned int nCols = 0;
	

	const char* header = "(C) Pawlin Technologies Ltd. Binary Spreadsheets file format.\n";
	size_t bufsize = strlen(header);
	char* buf = new char[bufsize];
	fread(buf, sizeof(char), bufsize, file);
	delete [] buf;
	fread(&nRows, sizeof(unsigned int), 1, file);
	fread(&nCols, sizeof(unsigned int), 1, file);
	
	rows.resize(nRows);
	for (unsigned int i = 0; i < rows.size(); i++) {
		rows[i].resize(nCols);
	}	

	for(unsigned i = 0; i < rows.size(); i++) {
		fread(&rows[i].data[0], sizeof(float), nCols, file);
	}
	fclose(file);
	
	return true;*/
}


#define CSV_LINE_BUFFSIZE 20000
bool Dataset::loadCSV(const char *filename, bool useComma) {
	throw std::runtime_error("don't use this. Use TXTReader::init or constructor to load csv, the implementation below is wrong");
/*	char linebuffer[CSV_LINE_BUFFSIZE];

	vector < float > data;

	std::ifstream *file = new std::ifstream(filename);
	
	if(!file->good())  {
		return false;
	}
	size_t nRows = 0;
	size_t nCols = 0;	
	int counter = 0;
	while(!file->eof()) {
		file->getline(linebuffer, CSV_LINE_BUFFSIZE);
		if(file->gcount() < 3) {
			break;
		}
		string str(linebuffer);
		const size_t length = str.size();
		std::string data_item_str = "";
		size_t last_data_size = 0;
		for(size_t i = 0; i < length; ++i) {
			const char c = str[i];
			if((c == ';' && !useComma) || (c == ',' && useComma) || (i == (length - 1) || c=='\r' || c=='\n')) {
				float v = 0.0f;
				const bool success = string_to_number(data_item_str, v);
				if(success) {
					data.push_back(v);
					if(nRows == 0) {
						nCols++;
					}
				}
				else {
					printf("can't make number from this:[%s]\n", data_item_str.c_str());
					exit(-1);
				}
				data_item_str.clear();
			}
			else {
				data_item_str.push_back(c);
			}
		}
		if(data.size() != last_data_size) {
			nRows++;
		}
		last_data_size = data.size();
	}

	rows.resize(nRows);

	for (size_t i = 0; i < rows.size(); i++) {
		rows[i].resize(nCols);
	}

	for(size_t i = 0; i < nRows; i++) {
		for(size_t j = 0; j < nCols; j++) {
			rows[i].data[j] = data[i*nCols + j];
		}
	}

	data.clear();
	file->close();
	delete file;
	
	return true;
	*/
}

void VectorMatrix::init(const std::vector<FloatMatrix> &a) {
	elements.resize(a.size());
	size_t offset = 0;
	for(size_t i = 0; i < elements.size(); i++) {
		size_t rows = a[i].size();
		size_t cols = a[i].at(0).data.size();
		elements[i].rows = rows;
		elements[i].cols = cols;
		elements[i].offset = offset;
		offset += rows*cols;
	}
	data.resize(offset);
	//printf("e,r,c = %d,%d,%d\n",elements,rows,cols);
	for(size_t i = 0; i < elements.size(); i++) {
		for(size_t j = 0; j < elements[i].rows; j++)
			memcpy(this->getRow(i,j),&(a[i][j].data[0]), sizeof(float)*a[i][j].data.size());
	}
}

FloatVector *FloatVector::newArray(size_t size, size_t dims, float initial_value) { // was introduced to use with CycleBufferGeneric template class
	FloatVector etalon(dims,initial_value);
	return etalon.newArrayOfClones(size);
}

NamedStringTable::NamedStringTable(const NamedDataset& nd) {
	this->setNames(nd.getNames());
	this->setGroups(nd.getGroups());
	this->setRowNames(nd.getRowNames());
	const size_t row_count = nd.getData().size();
	const size_t col_count = (row_count == 0) ? 0 : nd.getData().rows[0].data.size();
	StringTable st_data;
	st_data.rows.resize(row_count);
	for(size_t i = 0; i < row_count; ++i) {
		vector<string> new_row(col_count);
		for(size_t j = 0; j < col_count; ++j) {
			const std::string word = std::to_string(nd.getData().rows[i].data[j]);
			new_row[j] = word;
		}
		st_data.rows[i] = new_row;
	}
	this->setData(st_data);
}

void FloatVector::join(const FloatVector &other) {
	data.insert(data.end(),other.data.begin(),other.data.end());
}

float FloatVector::sqdiff(const FloatVector &other) const {

	const float *df = &(data.front());
	const float *of = &(other.data.front());
	 size_t size = std::min<size_t>(data.size(),other.data.size());
	 float sum4 = 0,dif4;
	 size_t s=0;
#ifdef SSE
	if(size>=8) { // otherwise too much overhead
		const bool aligned = true;// FloatVector now is always aligned ;//((((int)df)&15)==0 && (((int)of)&15)==0);
		const float *xa = aligned ? df : (float *)_aligned_malloc(sizeof(float)*size*2,16);
		const float *xb = aligned ? of : xa + size;
		if(!aligned) {
			memcpy((float *)xa,df,sizeof(float)*size); // align memory for reading
			memcpy((float *)xb,of,sizeof(float)*size); // align memory for reading
		}
		ALIGN_TYPE float xmm_RAM[4] = {0,0,0,0}; //temp array for sse

		__m128 xmm_a;
		__m128 xmm_b;
		__m128 xmm_c;
		__m128 xmm_sum;
		xmm_sum  = _mm_load1_ps(xmm_RAM);

			for(; s+4 < size; s+=4) {
				xmm_a  = _mm_load_ps(xa+s);
				xmm_b  = _mm_load_ps(xb+s);
				xmm_c  = _mm_sub_ps(xmm_a,xmm_b);
				xmm_c  = _mm_mul_ps(xmm_c,xmm_c);
				xmm_sum  = _mm_add_ps(xmm_sum,xmm_c);
			}
		_mm_store_ps(xmm_RAM,xmm_sum);
		sum4 += xmm_RAM[0] + xmm_RAM[1] + xmm_RAM[2] + xmm_RAM[3];
		if(!aligned) _aligned_free((float *)xa); // xb is part of it
	}
#endif
	for(; s < size; s++) {
		dif4 = df[s]-of[s];
		sum4 += dif4*dif4;
	}
	return sum4;
}

void FloatVector::normalize(const float eps)
{
	float norma = norm(); if (norma < eps) norma = eps;
	mul(1.0f / norma);
}

#ifdef USE_OPTIMIZED_STRINGVECTOR
size_t StringVector::maxline = 16;
#endif