#include <QCoreApplication>
#include <iostream>

// Forward declarations
void test_base36();
void test_channel_type();
void test_constant_bpm();
void test_bpm_change();
void test_parse_header();
void test_parse_channel_row();
void test_selected_note_info();

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    test_base36();
    test_channel_type();
    test_constant_bpm();
    test_bpm_change();
    test_parse_header();
    test_parse_channel_row();
    test_selected_note_info();

    std::cout << "\nAll tests passed.\n";
    return 0;
}
