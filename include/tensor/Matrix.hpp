#ifndef HDB_CT_MATRIX_HPP
#define HDB_CT_MATRIX_HPP

#include "Vector.hpp"

namespace hdb {

template <unsigned int aRows, unsigned int aColumns, unsigned int bRows, unsigned int bColumns>
concept isMatrixSmaller = requires {
	aRows <= bRows;
	aColumns <= bColumns;
};

template <unsigned int aColumns, unsigned int bRows>
concept canMultiplyMatrices = requires {
	aColumns == bRows;
};

template <unsigned int rows, unsigned int columns>
class Matrix {
public:
	constexpr Matrix() {}

	constexpr Matrix(float s) {
		for (unsigned int row = 0; row < rows; ++row) {
			for (unsigned int column = 0; column < columns; ++column) {
				data[row][column] = s;
			}
		}
	}

	template <unsigned int fRows, unsigned int fColumns> requires isMatrixSmaller<rows, columns, fRows, fColumns>
	constexpr Matrix(Matrix<fRows, fColumns> matrix) {
		for (unsigned int row = 0; row < rows; ++row) {
			for (unsigned int column = 0; column < columns; ++column) {
				data[row][column] = matrix[row][column];
			}
		}	
	}

	template <unsigned int fRows, unsigned int fColumns> requires isMatrixSmaller<rows, columns, fRows, fColumns>
	constexpr Matrix<rows, columns> operator=(Matrix<fRows, fColumns> matrix) {
		for (unsigned int row = 0; row < rows; ++row) {
			for (unsigned int column = 0; column < columns; ++column) {
				data[row][column] = matrix[row][column];
			}
		}
		
		return *this;
	}

	constexpr unsigned int getRows() const { return rows; }
	constexpr unsigned int getColumns() const { return columns; }

	constexpr float* const operator[](unsigned int row) {
		return data[row];
	}

	template <unsigned int fRows, unsigned int fColumns> requires canMultiplyMatrices<columns, fRows>
	constexpr Matrix<rows, fColumns> operator*(Matrix<fRows, fColumns> factor) {
		Matrix<rows, fColumns> result(1.0f);

		for (unsigned int row = 0; row < rows; ++row) {
			for (unsigned int fColumn = 0; fColumn < fColumns; ++fColumn) {
				float sum = 0.0f;

				for (unsigned int i = 0; i < columns; ++i) {
					sum += data[row][i] * factor.data[i][fColumn];
				}

				result.data[row][fColumn] = sum;
			}
		}

		return result;
	}

	template <unsigned int dimension> requires requires {
		dimension == rows;
		dimension == columns;
	}
	constexpr Vector<dimension> operator*(Vector<dimension> factor) {
		Vector<dimension> result;

		for (unsigned int row = 0; row < dimension; ++row) {
			float sum = 0.0f;

			for (unsigned int column = 0; column < dimension; ++column) {
				sum += factor[column] * data[row][column];
			}

			result[row] = sum;
		}

		return result;
	}

	

	float data[rows][columns];
};

typedef Matrix<2, 2> Matrix2;
typedef Matrix<3, 3> Matrix3;
typedef Matrix<4, 4> Matrix4;

}

#endif