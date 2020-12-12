// (c) Pawlin Technologies Ltd. 2009-2010 
// http://www.pawlin.ru 
// 
// File: Simpematrix.h
// Purpose: SimpleMatrix class
// Author:  Dolgopolov A.V. 
// 
// ALL RIGHTS RESERVED. USAGE OF THIS FILE FOR ANY REASON 
// WITHOUT WRITTEN PERMISSION FROM 
// PAWLIN TECHNOLOGIES LTD IS PROHIBITED 
// ANY VIOLATION OF THE COPYRIGHT AND PATENT LAW WILL BE PROSECUTED 
// FOR ADDITIONAL INFORMATION REFER http://www.pawlin.ru 
// ----------------------------------------------
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <PWNGeneral/PWNExceptionBasic.h>
#include <PWNGeneral/pwnutil.h>
#ifndef WIN32
#include <stdint.h>
#endif
using std::vector;

class SimpleMatrix {
	
	vector <float> data;
	float* work_data;
	bool assigned;
	
	

public:
	size_t cols, rows;
	SimpleMatrix() { clear(); }
	SimpleMatrix(size_t cols): cols(cols), work_data(0), assigned(false) {
		rows=0;		
	}
	SimpleMatrix(size_t rows, size_t cols) : cols(cols), rows(rows), assigned(false) {
		data.resize(cols*rows);
		work_data = data.empty() ? NULL : &this->data[0];
	}
	SimpleMatrix(size_t rows, size_t cols, float value) : cols(cols), rows(rows), assigned(false) {
		data.resize(cols*rows, value);
		work_data = data.empty() ? NULL : &this->data[0];
	}
	SimpleMatrix(size_t rows, size_t cols, float* data) : cols(cols), rows(rows) {
		assigned = true;
		work_data = data;
	}
	SimpleMatrix(const SimpleMatrix & other) {
		data = other.getData();
		rows = other.rows;
		cols = other.cols;
		assigned = false;
		work_data = data.empty() ? NULL : &this->data[0];
	}

  SimpleMatrix& operator=(const SimpleMatrix& other) {
    data = other.getData();
    rows = other.rows;
    cols = other.cols;
    assigned = false;
    work_data = data.empty() ? NULL : &this->data[0];
    return *this;
  }

	size_t getRows() const { return rows;}
	size_t getCols() const { return cols;}

	const vector <float> & getData() const { return data;}
	vector <float> & getData() { return data;}

	size_t getDataSize()const {return data.size();}

	void setData(const vector <float> & data, size_t rows, size_t cols){
		this->data.assign(data.begin(), data.end());
		this->rows = rows;
		this->cols = cols;
		work_data = &this->data[0];
		assigned = false;
	}
	void setData(float* data, size_t rows, size_t cols){
		work_data = data; // place of the potential memory leak!
		this->rows = rows;
		this->cols = cols;
		assigned = true;
	}

	float *getArray() { return work_data; }
	const float *getArray() const { return work_data; }
	float * operator [] (size_t cl) { return &work_data[cl*cols]; }
	const float *operator [](size_t cl) const { return &work_data[cl*cols]; }
	
	bool empty(void) const { return ((this->assigned == false) && (this->data.empty())); }
	
	bool isAssigned() const { return assigned;}
		
	void clear() {
		this->data.clear();
		this->rows = 0;
		this->cols = 0;
		this->work_data = NULL;
		this->assigned = false;
	}

	void setSize(size_t nrows, size_t ncols){
		data.resize(ncols*nrows);
		rows = nrows;
		cols = ncols;
		work_data = data.empty() ? NULL : &data[0];
		assigned = false;
	}
	void reserve(size_t size) {
		if (this->assigned) throw "simple matrix. Can't reserve.\n";

		this->data.reserve(size);
		if (!this->data.empty())	
			this->work_data = &this->data[0];
	}
	void addRow(const vector<float> & newRow) {
		if (this->assigned) throw("addRow: can`t add row");
		
		if (newRow.empty()) return;
		
		if (empty()) {
			this->data.insert(this->data.end(), newRow.begin(), newRow.end());
			this->work_data = &this->data[0];
			this->cols = newRow.size();
			this->rows = 1;
		}
		else {
			if (this->cols != newRow.size()) throw "Can't add row: incorrect size\n";
			this->data.insert(this->data.end(), newRow.begin(), newRow.end());
			this->work_data = &this->data[0];		
			this->rows ++;
		}
	}
	void addMatrix(const SimpleMatrix &mat) {
		if (assigned) throw("addMatrix: can`t add matrix as it it assigned");
		if(this->cols!=mat.cols) throw PWNExceptionBasic("addMatrix: can't add matrix as it has different number of columns");
		size_t cursz = data.size();
		data.resize(cursz+mat.data.size());
		memcpy(&data[cursz],&mat.data.front(),sizeof(float)*mat.data.size());
		rows+=mat.rows;
		work_data = &data[0];
	}
	void deleteRow(size_t nRow){
		if (assigned) throw("deleteRow: can`t delete row");
		if (nRow >= this->rows) 
			throw("Number of row more then rows in matrix");
		if (this->rows-1 == 0) throw("Rows in matrix can not be zero");
		vector<float>::iterator startRow = this->data.begin()+this->cols*nRow;
		this->data.erase(startRow, startRow+this->cols);
		this->rows--;
		work_data = &data[0];
	}
	void fillRow(const vector<float> & newRow, size_t rowNum) {
		float* pRow = getRow(rowNum);
		
		memcpy(pRow, &newRow[0], std::min<size_t>(cols, newRow.size())*sizeof(float));
	}
	float* getRow(size_t nRow){
		if (nRow >= this->rows) 
			throw("Number of row more then rows in matrix");
		return &this->work_data[this->cols*nRow];
	}
	 const float* getRowConst(size_t nRow) const{
		if (nRow >= this->rows)
			throw("Number of row more then rows in matrix");
		return &this->work_data[this->cols*nRow];
	};
	void setRandom (float a, float b) {
		for (size_t i=0; i< rows*cols; i++) {
			work_data[i] = (b-a)*(float)rand()/(float)RAND_MAX + a;
		}
	}
	void setIdentity(float v = 1.0f) {
		setZero();
		for(size_t i = 0; i < std::min<size_t>(cols,rows); i++)
			(*this)[i][i] = v;
	}
	void setZero() {
		if (work_data != 0)
			memset(&work_data[0],0,sizeof(float)*rows*cols);
	}

	SimpleMatrix getSubMatrix(int row, int col, int n_rows, int n_cols)const {
	  SimpleMatrix res;
	  res.setSize(n_rows, n_cols);
	  float* dst = res.getArray();
	  const float* src = getRowConst(row)+col;
	  for (int i = 0; i< n_rows; i++, dst += n_cols, src+= getCols()){
		  memcpy(dst, src, n_cols*sizeof(float));
	  }
	  return res;
  }


  void reshape(size_t n_rows){

	  size_t sz = getRows()*getCols();
	  rows = n_rows;
	  cols = sz/rows;
	  
  }


	void print(const char *name) {
		printf("Matrix %s\n", name);
		for(size_t i = 0; i <  rows; i++) {
			for(size_t j = 0; j <  cols; j++)
				printf("%.6f ", work_data[i*cols+j]);
			printf("\n");
		}
	}

	bool loadBIN(const char * filename) {
		FILE * f = fopen(filename, "rb");
		if (!f) return false;
		bool res = loadBIN(f);
		fclose(f);
		return res;
	}
	bool saveBIN(const char * filename) const {
		if (this->assigned) return false;	
		FILE * f = fopen(filename, "wb");
		if (!f) return false;
		bool res = saveBIN(f);
		fclose(f);
		return res;
	}
	bool loadBIN(FILE * f) {
		if (!f) return false;			
		uint64_t bf;		
		if (fread(&bf, sizeof(bf), 1, f) != 1) return false;
		this->cols = size_t(bf);
		if (fread(&bf, sizeof(bf), 1, f) != 1) return false;
		this->rows = size_t(bf);		
		if (fread(&bf, sizeof(bf), 1, f) != 1) return false;
		size_t data_size = size_t(bf);
		this->data.resize(data_size);
		if (data_size > 0) {
			if (fread(&this->data[0], sizeof(float), data_size, f) != data_size) return false;
			this->work_data = &this->data[0];		
		}
		else
			this->work_data = NULL;

		this->assigned = false;

		return true;
	}
	bool saveBIN(FILE * f) const {
		if (this->assigned) return false;
		if (!f) return false;	
		uint64_t bf = this->cols;		
		if (fwrite(&bf, sizeof(bf), 1, f) != 1) return false;
		bf = this->rows;		
		if (fwrite(&bf, sizeof(bf), 1, f) != 1) return false;		
		bf = this->data.size();	
		if (fwrite(&bf, sizeof(bf), 1, f) != 1) return false;
		if (bf > 0) {
			if (fwrite(&this->data[0], sizeof(float), (size_t) bf, f) != bf) return false;
		}		
		return true;	
	}

	void saveText(FILE *file) const {
		fprintf(file,"SimpleMatrix\n");
		fprintf(file,"rows:%zu\n", (size_t) rows);
		fprintf(file,"cols:%zu\n", (size_t) cols);
		fprintf(file,"size:%zu\n", (size_t) data.size());
		FOR_ALL(data,i) fprintf(file,"%.9f ",data[i]);
		fprintf(file,"\n");
	}

	void loadText(FILE *file) {
		size_t bf = 0;		
		char buf[1024];
		fgets(buf,1024,file);
		if(strstr(buf,"SimpleMatrix")==NULL) throw("wrong simple matrix file format, SimpleMatrix tag expected");
		fscanf(file,"rows:%zu\n",&bf);
		rows = (size_t) bf;
		fscanf(file,"cols:%zu\n", &bf);
		cols = (size_t) bf;
		fscanf(file,"size:%zu\n", &bf);
		data.resize((size_t) bf);
		FOR_ALL(data,i) fscanf(file,"%f",&data[i]);
		this->work_data = &this->data[0];		
		this->assigned = false;
	}
void print_header(const char *name)const {
		printf("Matrix %s rows:%zu cols:%zu size:%zu assigned:%s\n", name, getRows(), getCols(), data.size(), assigned ? "true":"false");
	}


	void copyCol(int idx_col, SimpleMatrix & b, int idx_col_b){

		float* pA = getArray()+idx_col;
		float* pB = b.getArray()+idx_col_b;
		int ncols_a = (int)getCols();
		int ncols_b = (int)b.getCols();
		for (size_t i = 0; i< getRows(); i++, pA+= ncols_a, pB+= ncols_b){
			pB[0] = pA[0];
		}
	}

	void scaleCol(int idx_col, float alpha = 1.0f){
		float* pA = getArray()+idx_col;
		int ncols_a = (int)getCols();
		for (size_t i = 0; i< getRows(); i++, pA+= ncols_a){
			pA[0] *= alpha;
		}
	}



	~SimpleMatrix() {				
	}
};