/*
    ==============================================================

    SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
    http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/

    Copyright (C) Intel Corporation

    THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
    NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.

    =============================================================
*/

class Matrix {
    static const int n = 20;
    float array[n][n];
public:
    Matrix() {}
    Matrix( float z ) {
        for( int i=0; i<n; ++i )
            for( int j=0; j<n; ++j )
                array[i][j] = i==j ? z : 0;
    }
    friend Matrix operator-( const Matrix& x ) {
        Matrix result;
        for( int i=0; i<n; ++i )
            for( int j=0; j<n; ++j )
                result.array[i][j] = -x.array[i][j];
        return result;
    }
    friend Matrix operator+( const Matrix& x, const Matrix& y ) {
        Matrix result;
        for( int i=0; i<n; ++i )
            for( int j=0; j<n; ++j )
                result.array[i][j] = x.array[i][j] + y.array[i][j];
        return result;
    }
    friend Matrix operator-( const Matrix& x, const Matrix& y ) {
        Matrix result;
        for( int i=0; i<n; ++i )
            for( int j=0; j<n; ++j )
                result.array[i][j] = x.array[i][j] - y.array[i][j];
        return result;
    }
    friend Matrix operator*( const Matrix& x, const Matrix& y ) {
        Matrix result(0);
        for( int i=0; i<n; ++i ) 
            for( int k=0; k<n; ++k )
                for( int j=0; j<n; ++j )
                    result.array[i][j] += x.array[i][k] * y.array[k][j];
        return result;
    }
};
