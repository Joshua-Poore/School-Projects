#include <iostream>
using namespace std;

int main() {
    cout << "Joshua Poore" << endl;
    printf("Joshua Poore");

    cout << endl;  // For proper spacing

    cout << 25/7 << endl;
    printf("%d", 25/7);

    cout << endl;  // For proper spacing

    cout << 25.0/7.0 << endl;
    printf("%g", 25.0/7.0);

    // The reason the results differ is because printf does not move to the next line
    // So the next cout afterwords prints onto the same line.

    // But I assume that what the question actually means is why the 25/7 and 25.0/7.0 results are different.
    // In that case, this is because the first operation, 25/7 causes implict narrowing, which essentially means that it cuts off any remainder.
    // This happens here because the machine sees 25/7 as int/int which it wants to only be whole numbers so implict narrowing occurs.
    // The reason it does not occur on the 25.0/7.0 is because the machine sees that as double/double which it does want to keep the decimal places of (to a certain point) so it keeps the remainder.

    return 0;
}
