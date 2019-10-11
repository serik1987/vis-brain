//
// Created by serik1987 on 21.09.19.
//

#include "../Application.h"
#include "../data/Matrix.h"
#include "../data/LocalMatrix.h"
#include "../data/ContiguousMatrix.h"

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

    logging::progress(0, 1, "Addition test");
    logging::enter();
    logging::debug("*(a+2) = " + to_string(*(a+2)));
    logging::debug("*(b+2) = " + to_string(*(b+2)));
    logging::debug("*(c+2) = " + to_string(*(c+2)));
    logging::debug("*(d+2) = " + to_string(*(d+2)));
    logging::debug("*(2+aa) = " + to_string(*(2+aa)));
    logging::debug("*(2+bb) = " + to_string(*(2+bb)));
    logging::debug("*(2+cc) = " + to_string(*(2+cc)));
    logging::debug("*(2+dd) = " + to_string(*(2+dd)));
    logging::debug("After addition: ");
    logging::debug("*a = " + to_string(*a));
    logging::debug("*b = " + to_string(*b));
    logging::debug("*c = " + to_string(*c));
    logging::debug("*d = " + to_string(*d));
    logging::debug("*aa = " + to_string(*aa));
    logging::debug("*bb = " + to_string(*bb));
    logging::debug("*cc = " + to_string(*cc));
    logging::debug("*dd = " + to_string(*dd));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Substraction test");
    logging::enter();
    logging::debug("*(a-2) = " + to_string(*(a-2)));
    logging::debug("*(b-2) = " + to_string(*(b-2)));
    logging::debug("*(c-2) = " + to_string(*(c-2)));
    logging::debug("*(d-2) = " + to_string(*(d-2)));
    logging::debug("a-aa = " + to_string(a-aa));
    logging::debug("b-bb = " + to_string(b-bb));
    logging::debug("c-cc = " + to_string(c-cc));
    logging::debug("d-dd = " + to_string(d-dd));
    logging::debug("After substraction:");
    logging::debug("*a = " + to_string(*a));
    logging::debug("*b = " + to_string(*b));
    logging::debug("*c = " + to_string(*c));
    logging::debug("*d = " + to_string(*d));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Less operator");
    logging::enter();
    logging::debug("\ta\tb\tc\td\t");
    logging::debug("a\t" + to_string(a<a) + "\t" + to_string(a<b) + "\t" + to_string(a<c) + "\t" + to_string(a<d));
    logging::debug("b\t" + to_string(b<a) + "\t" + to_string(b<b) + "\t" + to_string(b<c) + "\t" + to_string(b<d));
    logging::debug("c\t" + to_string(c<a) + "\t" + to_string(c<b) + "\t" + to_string(c<c) + "\t" + to_string(c<d));
    logging::debug("d\t" + to_string(d<a) + "\t" + to_string(d<b) + "\t" + to_string(d<c) + "\t" + to_string(d<d));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Less or equal operator");
    logging::enter();
    logging::debug("\ta\tb\tc\td\t");
    logging::debug("a\t" + to_string(a<=a) + "\t" + to_string(a<=b) + "\t" + to_string(a<=c) + "\t" + to_string(a<=d));
    logging::debug("b\t" + to_string(b<=a) + "\t" + to_string(b<=b) + "\t" + to_string(b<=c) + "\t" + to_string(b<=d));
    logging::debug("c\t" + to_string(c<=a) + "\t" + to_string(c<=b) + "\t" + to_string(c<=c) + "\t" + to_string(c<=d));
    logging::debug("d\t" + to_string(d<=a) + "\t" + to_string(d<=b) + "\t" + to_string(d<=c) + "\t" + to_string(d<=d));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Greater operator");
    logging::enter();
    logging::debug("\ta\tb\tc\td\t");
    logging::debug("a\t" + to_string(a>a) + "\t" + to_string(a>b) + "\t" + to_string(a>c) + "\t" + to_string(a>d));
    logging::debug("b\t" + to_string(b>a) + "\t" + to_string(b>b) + "\t" + to_string(b>c) + "\t" + to_string(b>d));
    logging::debug("c\t" + to_string(c>a) + "\t" + to_string(c>b) + "\t" + to_string(c>c) + "\t" + to_string(c>d));
    logging::debug("d\t" + to_string(d>a) + "\t" + to_string(d>b) + "\t" + to_string(d>c) + "\t" + to_string(d>d));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Greater operator");
    logging::enter();
    logging::debug("\ta\tb\tc\td\t");
    logging::debug("a\t" + to_string(a>=a) + "\t" + to_string(a>=b) + "\t" + to_string(a>=c) + "\t" + to_string(a>=d));
    logging::debug("b\t" + to_string(b>=a) + "\t" + to_string(b>=b) + "\t" + to_string(b>=c) + "\t" + to_string(b>=d));
    logging::debug("c\t" + to_string(c>=a) + "\t" + to_string(c>=b) + "\t" + to_string(c>=c) + "\t" + to_string(c>=d));
    logging::debug("d\t" + to_string(d>=a) + "\t" + to_string(d>=b) + "\t" + to_string(d>=c) + "\t" + to_string(d>=d));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Compound operator test");
    logging::enter();
    logging::debug("Before application:");
    logging::debug("*a = " + to_string(*a));
    logging::debug("*b = " + to_string(*b));
    logging::debug("*c = " + to_string(*c));
    logging::debug("*d = " + to_string(*d));
    logging::debug("Assignment result:");
    logging::debug("*(a+=2) = " + to_string(*(a+=2)));
    logging::debug("*(b+=2) = " + to_string(*(b+=2)));
    logging::debug("*(c+=2) = " + to_string(*(c+=2)));
    logging::debug("*(d+=2) = " + to_string(*(d+=2)));
    logging::debug("After assignment:");
    logging::debug("*a = " + to_string(*a));
    logging::debug("*b = " + to_string(*b));
    logging::debug("*c = " + to_string(*c));
    logging::debug("*d = " + to_string(*d));
    logging::exit();

    logging::progress(0, 1, "Compound operator test");
    logging::enter();
    logging::debug("Before assignment:");
    logging::debug("*aa = " + to_string(*aa));
    logging::debug("*bb = " + to_string(*bb));
    logging::debug("*cc = " + to_string(*cc));
    logging::debug("*dd = " + to_string(*dd));
    logging::debug("Assignment result:");
    logging::debug("*(aa-=2) = " + to_string(*(aa-=2)));
    logging::debug("*(bb-=2) = " + to_string(*(bb-=2)));
    logging::debug("*(cc-=2) = " + to_string(*(cc-=2)));
    logging::debug("*(dd-=2) = " + to_string(*(dd-=2)));
    logging::debug("After assignment:");
    logging::debug("*aa = " + to_string(*aa));
    logging::debug("*bb = " + to_string(*bb));
    logging::debug("*cc = " + to_string(*cc));
    logging::debug("*dd = " + to_string(*dd));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Random access test (rvalue)");
    logging::enter();
    logging::debug("aa[4] = " + to_string(aa[4]));
    logging::debug("bb[4] = " + to_string(bb[4]));
    logging::debug("cc[4] = " + to_string(cc[4]));
    logging::debug("dd[4] = " + to_string(dd[4]));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Random access test (lvalue)");
    aa[4] = 0.0;
    // bb[4] = 0.0; Compilation error
    cc[4] = 0.0;
    // dd[4] = 0.0; Compilation error
    logging::enter();
    logging::debug("Matrix A");
    A.printLocal();
    logging::debug("Matrix B");
    B.printLocal();
    logging::exit();

    logging::progress(0, 1, "val(...) test (rvalue)");
    logging::enter();
    logging::debug("aa[2, 3] = " + to_string(aa.val(2, 3)));
    logging::debug("bb[2, 3] = " + to_string(bb.val(2, 3)));
    logging::debug("cc[2, 3] = " + to_string(cc.val(2, 3)));
    logging::debug("dd[2, 3] = " + to_string(dd.val(2, 3)));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "val(...) test (lvalue)");
    aa.val(-1, -5) = 0.8;
    // bb.val(-1, -5) = 0.8; Compilation error
    cc.val(-1, -5) = 0.8;
    // dd.val(-1, -5) = 0.8; Compilation error
    logging::enter();
    logging::debug("Matrix A");
    A.printLocal();
    logging::debug("Matrix B");
    B.printLocal();
    logging::exit();

    logging::progress(0, 1, "SWAP test");
    logging::enter();
    logging::debug("Before swap:");
    logging::debug("*aa = " + to_string(*aa));
    logging::debug("*bb = " + to_string(*bb));
    logging::debug("*cc = " + to_string(*cc));
    logging::debug("*dd = " + to_string(*dd));
    // swap(aa, bb); Compilation error
    // swap(cc, dd); Compilation error
    swap(aa, cc);
    logging::debug("After swap:");
    logging::debug("*aa = " + to_string(*aa));
    logging::debug("*bb = " + to_string(*bb));
    logging::debug("*cc = " + to_string(*cc));
    logging::debug("*dd = " + to_string(*dd));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Standard iterators test (reading)");
    logging::enter();
    logging::debug("Test on LocalMatrix");
    logging::debug("begin: " + to_string(*A.begin()));
    logging::debug("cbegin: " + to_string(*A.cbegin()));
    logging::debug("end: " + to_string(*(A.end()-1)));
    logging::debug("cend: " + to_string(*(A.cend()-1)));
    logging::debug("Test on Matrix:");
    logging::debug("begin: " + to_string(*B.begin()));
    logging::debug("cbegin: " + to_string(*B.cbegin()));
    logging::debug("end: " + to_string(*(B.end()-1)));
    logging::debug("cend: " + to_string(*(B.end()-1)));
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Looping test (reading)");
    logging::enter();
    logging::debug("Test on LocalMatrix, r/w iterator");
    for (auto a = A.begin(); a != A.end(); ++a){
        logging::debug("A[" + to_string(a.getRow()) + "," + to_string(a.getColumn()) + "] = " + to_string(*a));
    }
    logging::debug("");
    logging::exit();
    logging::enter();
    logging::debug("Test on LocalMatrix, r/o iterator");
    for (auto a = A.cbegin(); a != A.cend(); ++a){
        logging::debug("A[" + to_string(a.getRow()) + "," + to_string(a.getColumn()) + "] = " + to_string(*a));
    }
    logging::debug("");
    logging::exit();
    logging::enter();
    logging::debug("Test on ContinuousMatrix, r/w iterator");
    for (auto a = B.begin(); a != B.end(); ++a){
        logging::debug("B[" + to_string(a.getRow()) + "," + to_string(a.getColumn()) + "] = " + to_string(*a));
    }
    logging::debug("");
    logging::exit();
    logging::enter();
    logging::debug("Test on ContinuousMatrix, r/o iterator");
    for (auto a = B.cbegin(); a != B.cend(); ++a){
        logging::debug("B[" + to_string(a.getRow()) + "," + to_string(a.getColumn()) + "] = " + to_string(*a));
    }
    logging::debug("");
    logging::exit();

    double x = 0.0;
    for (auto a = A.begin(); a != A.end(); ++a){
        *a = ++x;
    }

    /* Compilation error
    for (auto a = A.cbegin(); a != A.cend(); ++a){
        *a = ++x;
    }
     */

    for (auto b = B.begin(); b != B.end(); ++b){
        *b = ++x;
    }

    /* Compilation error
    for (auto b = B.cbegin(); b != B.cend(); ++b){
        *b = ++x;
    }
     */

    logging::enter();
    logging::debug("Matrix A, after second initialization");
    A.printLocal();
    logging::debug("");
    logging::debug("Matrix B, after second initialization");
    B.printLocal();
    logging::debug("");
    logging::exit();


    logging::progress(1, 1);
}