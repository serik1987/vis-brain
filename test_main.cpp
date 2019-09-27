//
// Created by serik1987 on 21.09.19.
//

#include "Application.h"
#include "data/Matrix.h"
#include "data/LocalMatrix.h"
#include "data/ContiguousMatrix.h"

void fill_matrix(data::Matrix& matrix, double filler = 0){
    for (int i=0; i < matrix.getHeight(); i++){
        for (int j=0; j < matrix.getWidth(); j++){
            int index = i * matrix.getWidth() + j;
            if (index >= matrix.getIstart() && index < matrix.getIfinish()){
                if (filler == 0) {
                    matrix.getValue(i, j) = 100 * i + j;
                } else {
                    matrix.getValue(i, j) = filler;
                }
            }
        }
    }
}

void print_iterator_properties(const data::Matrix::AbstractIterator& x){
    using namespace std;

    logging::debug("Current index: " + to_string(x.getIndex()));
    logging::debug("Current row: " + to_string(x.getRow()));
    logging::debug("Current column: " + to_string(x.getColumn()));
    logging::debug("Current row (um): " + to_string(x.getRowUm()));
    logging::debug("Current column (um): " + to_string(x.getColumnUm()));
}

void test_main(){
    using namespace std;

    Application& app = Application::getInstance();
    mpi::Communicator& comm = app.getAppCommunicator();

    logging::progress(0, 1, "Matrix initialization");
    data::LocalMatrix A(comm, 21, 21, 1.0, 1.0);
    fill_matrix(A);
    logging::enter();
    A.printLocal();
    logging::exit();

    data::ContiguousMatrix B(comm, 21, 21, 1.0, 1.0);
    fill_matrix(B);
    B.synchronize();
    logging::enter();
    B.printLocal();
    logging::exit();

    logging::progress(0, 1, "Iterator initialization");
    data::LocalMatrix::Iterator a(A, 20);
    data::LocalMatrix::ConstantIterator b(A, 30);
    data::ContiguousMatrix::Iterator c(B, 40);
    data::ContiguousMatrix::ConstantIterator d(B, 30);

    logging::progress(0, 1, "Reading iterator properties");
    logging::enter();
    logging::debug("si rw iterator (local matrix)");
    print_iterator_properties(a);
    logging::debug("si ro iterator (local matrix)");
    print_iterator_properties(b);
    logging::debug("si rw iterator (contiguous matrix)");
    print_iterator_properties(c);
    logging::debug("si ro iterator (contiguous matrix)");
    print_iterator_properties(d);
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Double index initialization");
    data::LocalMatrix::Iterator aa(A, 2, 8);
    data::LocalMatrix::ConstantIterator bb(A, 3, 5);
    data::ContiguousMatrix::Iterator cc(B, 4, 6);
    data::ContiguousMatrix::ConstantIterator dd(B, 5, 0);
    logging::enter();
    logging::debug("di rw iterator (local matrix)");
    print_iterator_properties(aa);
    logging::debug("di ro iterator (local matrix)");
    print_iterator_properties(bb);
    logging::debug("di rw iterator (contiguous matrix)");
    print_iterator_properties(cc);
    logging::debug("di ro iterator (contiguous matrix)");
    print_iterator_properties(dd);
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Default initialization");
    data::LocalMatrix::Iterator a0(A);
    data::LocalMatrix::ConstantIterator b0(A);
    data::ContiguousMatrix::Iterator c0(B);
    data::ContiguousMatrix::ConstantIterator d0(B);
    logging::enter();
    logging::debug("i0 rw iterator (local matrix)");
    print_iterator_properties(a0);
    logging::debug("i0 ro iterator (local matrix)");
    print_iterator_properties(b0);
    logging::debug("i0 rw iterator (contiguous matrix)");
    print_iterator_properties(c0);
    logging::debug("i0 ro iterator (local matrix)");
    print_iterator_properties(d0);
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Copy constructor");
    data::LocalMatrix::Iterator a1 = a;
    data::LocalMatrix::ConstantIterator b1 = b;
    data::ContiguousMatrix::Iterator c1 = c;
    data::ContiguousMatrix::ConstantIterator d1 = d;
    logging::enter();
    logging::debug("si rw iterator (local matrix); copy");
    print_iterator_properties(a1);
    logging::debug("si ro iterator (local matrix); copy");
    print_iterator_properties(b1);
    logging::debug("si rw iterator (contiguous matrix); copy");
    print_iterator_properties(c1);
    logging::debug("si ro iterator (contiguous matrix); copy");
    print_iterator_properties(d1);
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Copy operator");
    data::LocalMatrix::Iterator aa1(A);
    data::LocalMatrix::ConstantIterator bb1(A);
    data::ContiguousMatrix::Iterator cc1(B);
    data::ContiguousMatrix::ConstantIterator dd1(B);
    aa1 = aa, bb1 = bb, cc1 = cc, dd1 = dd;
    logging::enter();
    logging::debug("di rw iterator (local matrix); copy");
    print_iterator_properties(aa1);
    logging::debug("di ro iterator (local matrix); copy");
    print_iterator_properties(bb1);
    logging::debug("di rw iterator (contiguous matrix); copy");
    print_iterator_properties(cc1);
    logging::debug("di ro iterator (contiguous matrix); copy");
    print_iterator_properties(dd1);
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "equality operation");
    logging::enter();
    logging::debug("a vs a1: " + to_string(a == a1));
    logging::debug("a vs aa: " + to_string(a == aa));
    // logging::debug("a vs b: " + to_string(a == b)); No match for operator==
    logging::debug("a vs c: " + to_string(a == c));
    // logging::debug("a vs d: " + to_string(a == d)); No match for operator==
    logging::debug("a1 vs a: " + to_string(a1 == a));
    logging::debug("b vs b1: " + to_string(b == b1));
    logging::debug("b vs bb: " + to_string(b == bb));
    // logging::debug("b vs a: " + to_string(b == a)); No match for operator==
    // logging::debug("b vs c: " + to_string(b == c)); Invalid operands for binary expression
    logging::debug("b vs d: " + to_string(b == d));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "inequality operation");
    logging::enter();
    logging::debug("a vs a1: " + to_string(a != a1));
    logging::debug("a vs aa: " + to_string(a != aa));
    // logging::debug("a vs b: " + to_string(a != b));
    logging::debug("a vs c: " + to_string(a != c));
    // logging::debug("a vs d: " + to_string(a != d));
    logging::debug("a1 vs a: " + to_string(a1 != a));
    logging::debug("b vs b1: " + to_string(b != b1));
    logging::debug("b vs bb: " + to_string(b != bb));
    // logging::debug("b vs a: " + to_string(b != a));
    // logging::debug("b vs c: " + to_string(b != c));
    logging::debug("b vs d: " + to_string(b != d));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Operator dereferencing (rvalue)");
    logging::enter();
    logging::debug("*a = " + to_string(*a));
    logging::debug("*a1 = " + to_string(*a1));
    logging::debug("*b = " + to_string(*b));
    logging::debug("*b1 = " + to_string(*b1));
    logging::debug("*c = " + to_string(*c));
    logging::debug("*c1 = " + to_string(*c1));
    logging::debug("*d = " + to_string(*d));
    logging::debug("*d1 = " + to_string(*d1));
    logging::debug("*aa = " + to_string(*aa));
    logging::debug("*aa1 = " + to_string(*aa1));
    logging::debug("*bb = " + to_string(*bb));
    logging::debug("*bb1 = " + to_string(*bb1));
    logging::debug("*cc = " + to_string(*cc));
    logging::debug("*cc1 = " + to_string(*cc1));
    logging::debug("*dd = " + to_string(*dd));
    logging::debug("*dd1 = " + to_string(*dd1));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Operator dereferencing (lvalue)");
    *a = 1.1;
    *a1 = 1.2;
    // *b = 1.3; Compilation error
    // *b1 = 1.4; Compilation error
    *c = 1.5;
    *c1 = 1.6;
    // *d = 1.7; Compilation error
    // *d1 = 1.8; Compilation error
    *aa = 1.9;
    *aa1 = 2.0;
    // *bb = 2.1; Compilation error
    // *bb1 = 2.2; Compilation error
    *cc = 2.3;
    *cc1 = 2.4;
    // *dd = 2.5; Compilation error
    // *dd1 = 2.6; Compilation error

    logging::enter();
    logging::debug("Matrix A, after lvalue");
    A.printLocal();
    logging::debug("Matrix B, after lvalue");
    B.printLocal();
    logging::exit();

    logging::progress(0, 1, "Prefix increment");
    logging::enter();
    logging::debug("*(++a) = " + to_string(*++a));
    logging::debug("*(++aa) = " + to_string(*++aa));
    logging::debug("*(++c) = " + to_string(*++c));
    logging::debug("*(++cc) = " + to_string(*++cc));
    logging::debug("*(++b) = " + to_string(*++b));
    logging::debug("*(++bb) = " + to_string(*++bb));
    logging::debug("*(++d) = " + to_string(*++d));
    logging::debug("*(++dd) = " + to_string(*++dd));
    logging::debug("After increment: ");
    logging::debug("*a = " + to_string(*a));
    logging::debug("*aa = " + to_string(*aa));
    logging::debug("*c = " + to_string(*c));
    logging::debug("*cc = " + to_string(*cc));
    logging::debug("*d = " + to_string(*d));
    logging::debug("*dd = " + to_string(*dd));
    logging::debug("*b = " + to_string(*b));
    logging::debug("*bb = " + to_string(*bb));
    logging::debug("*d = " + to_string(*d));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Postfix increment");
    logging::enter();
    logging::debug("*a++ = " + to_string(*a++));
    logging::debug("*aa++ = " + to_string(*aa++));
    logging::debug("*b++ = " + to_string(*b++));
    logging::debug("*bb++ = " + to_string(*bb++));
    logging::debug("*c++ = " + to_string(*c++));
    logging::debug("*cc++ = " + to_string(*cc++));
    logging::debug("*d++ = " + to_string(*d++));
    logging::debug("*dd++ = " + to_string(*dd++));
    logging::debug("After increment: ");
    logging::debug("*a = " + to_string(*a));
    logging::debug("*aa = " + to_string(*aa));
    logging::debug("*b = " + to_string(*b));
    logging::debug("*bb = " + to_string(*bb));
    logging::debug("*c = " + to_string(*c));
    logging::debug("*cc++ = " + to_string(*cc));
    logging::debug("*d = " + to_string(*d));
    logging::debug("*dd = " + to_string(*dd));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Increment test");
    logging::enter();
    logging::debug("a == a1, stage 1: " + to_string(a == a1));
    logging::debug("a == a1, stage 2: " + to_string(a == ++++a1));
    logging::debug("b == b1, stage 1: " + to_string(b == b1));
    logging::debug("b == b1, stage 2: " + to_string(b == ++++b1));
    logging::debug("c == c1, stage 1: " + to_string(c == c1));
    logging::debug("c == c1, stage 2: " + to_string(c == ++++c1));
    logging::debug("d == d1, stage 1: " + to_string(d == d1));
    logging::debug("d == d1, stage 2: " + to_string(d == ++++d1));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Prefix decrement test");
    logging::enter();
    logging::debug("*--a = " + to_string(*--a));
    logging::debug("*--aa = " + to_string(*--aa));
    logging::debug("*--c = " + to_string(*--c));
    logging::debug("*--cc = " + to_string(*--cc));
    logging::debug("*--b = " + to_string(*--b));
    logging::debug("*--bb = " + to_string(*--bb));
    logging::debug("*--d = " + to_string(*--d));
    logging::debug("*--dd = " + to_string(*--dd));
    logging::debug("After decrement:");
    logging::debug("*a = " + to_string(*a));
    logging::debug("*aa = " + to_string(*aa));
    logging::debug("*c = " + to_string(*c));
    logging::debug("*cc = " + to_string(*cc));
    logging::debug("*b = " + to_string(*b));
    logging::debug("*bb = " + to_string(*bb));
    logging::debug("*d = " + to_string(*d));
    logging::debug("*dd = " + to_string(*dd));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Postfix decrement test");
    logging::enter();
    logging::debug("*a-- = " + to_string(*a--));
    logging::debug("*aa-- = " + to_string(*aa--));
    logging::debug("*c-- = " + to_string(*c--));
    logging::debug("*cc-- = " + to_string(*cc--));
    logging::debug("*b-- = " + to_string(*b--));
    logging::debug("*bb-- = " + to_string(*bb--));
    logging::debug("*d-- = " + to_string(*d--));
    logging::debug("*dd-- = " + to_string(*dd--));
    logging::debug("After decrement:");
    logging::debug("*a = " + to_string(*a));
    logging::debug("*aa = " + to_string(*aa));
    logging::debug("*c = " + to_string(*c));
    logging::debug("*cc = " + to_string(*cc));
    logging::debug("*b = " + to_string(*b));
    logging::debug("*bb = " + to_string(*bb));
    logging::debug("*d = " + to_string(*d));
    logging::debug("*dd-- = " + to_string(*dd));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Decrement comparison test");
    logging::enter();
    logging::debug("aa, test 1: " + to_string(aa==aa1));
    logging::debug("aa, test 2: " + to_string(--aa==--aa1));
    logging::debug("bb, test 1: " + to_string(bb==bb1));
    logging::debug("bb, test 2: " + to_string(--bb==--bb1));
    logging::debug("cc, test 1: " + to_string(cc==cc1));
    logging::debug("cc, test 2: " + to_string(--cc==--cc1));
    logging::debug("dd, test 1: " + to_string(dd==dd1));
    logging::debug("dd, test 2: " + to_string(--dd==--dd1));
    logging::debug("");
    logging::exit();


    logging::progress(1, 1);
}