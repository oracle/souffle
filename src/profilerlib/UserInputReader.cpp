/*
* Souffle - A Datalog Compiler
* Copyright (c) 2017, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/

//
// Created by Dominic Romanowski on 13/2/17.
//

#include "UserInputReader.hpp"

void InputReader::getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0) perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0) perror("tcsetattr ~ICANON");

    current_char = buf;
}

std::string InputReader::getInput() {
    output = "";
    current_char = 0;
    cursor_pos = 0;
    hist_pos = 0;
    tab_pos = 0;
    in_tab_complete = false;
    in_history = false;

    std::cout << this->prompt << std::flush;
    getch();

    bool escaped = false;
    bool arrow_key = false;

    while (current_char != '\n') {
        if (arrow_key) {
            moveCursor(current_char);
            escaped = false;
            arrow_key = false;
        } else if (escaped) {
            if (current_char == '[') {
                arrow_key = true;
            }
        } else {
            if (current_char == 27) {  // esc char for arrow keys
                escaped = true;
            } else if (current_char == '\t') {
                tabComplete();
            } else {
                if (in_history) {
                    output = current_hist_val;
                    in_history = false;

                } else if (in_tab_complete) {
                    output = current_tab_val;
                    in_tab_complete = false;
                }

                if (current_char == 127) {
                    backspace();
                } else {
                    output.insert(output.begin() + (cursor_pos++), current_char);
                    std::cout << current_char << std::flush;
                    showFullText(output);
                }
            }
        }

        getch();
    }

    if (in_history) {
        return current_hist_val;
    }
    if (in_tab_complete) {
        return current_tab_val;
    }

    return output;
}

void InputReader::setPrompt(std::string prompt) {
    this->prompt = prompt;
}

void InputReader::appendTabCompletion(std::vector<std::string> commands) {
    tab_completion.insert(std::end(tab_completion), std::begin(commands), std::end(commands));
}

void InputReader::appendTabCompletion(std::string command) {
    tab_completion.push_back(command);
}

void InputReader::tabComplete() {
    if (in_history) {
        output = current_hist_val;
        in_history = false;
    }
    if (!in_tab_complete) {
        current_tab_completes = std::vector<std::string>();
        original_tab_val = output;
        bool found_tab = false;
        for (auto& a : tab_completion) {
            if (a.find(original_tab_val) == 0) {
                current_tab_completes.push_back(a);
                found_tab = true;
            }
        }
        if (!found_tab) {
            std::cout << '\a' << std::flush;
            return;
        } else {
            in_tab_complete = true;
            tab_pos = 0;
            current_tab_val = current_tab_completes.at((unsigned)tab_pos);
            clearPrompt(output.size());

            cursor_pos = current_tab_val.size();
            std::cout << current_tab_val << std::flush;
        }
    } else {
        if (tab_pos + 1 >= current_tab_completes.size()) {
            clearPrompt(current_tab_val.size());
            current_tab_val = original_tab_val;
            in_tab_complete = false;
            cursor_pos = output.size();
            std::cout << output << std::flush;
        } else {
            tab_pos++;

            clearPrompt(current_tab_val.size());
            current_tab_val = current_tab_completes.at((unsigned)tab_pos);

            cursor_pos = current_tab_val.size();
            std::cout << current_tab_val << std::flush;
        }
    }
}

void InputReader::clearTabCompletion() {
    tab_completion = std::vector<std::string>();
}

void InputReader::clearHistory() {
    history = std::vector<std::string>();
}

void InputReader::addHistory(std::string hist) {
    for (auto& a : history) {
        if (hist.compare(a) == 0) {
            return;
        }
    }
    history.push_back(hist);
}

void InputReader::historyUp() {
    if (history.empty()) {
        std::cout << '\a' << std::flush;
        return;
    }
    if (in_tab_complete) {
        output = current_tab_val;
        in_tab_complete = false;
    }
    if (!in_history) {
        original_hist_val = output;
        original_hist_cursor_pos = cursor_pos;
        in_history = true;
        clearPrompt(output.size());
        hist_pos = history.size() - 1;
        current_hist_val = history.back();
        cursor_pos = current_hist_val.size();
        std::cout << current_hist_val << std::flush;
    } else {
        if (hist_pos > 0) {
            hist_pos--;
            clearPrompt(current_hist_val.size());
            current_hist_val = history.at((unsigned)hist_pos);
            cursor_pos = current_hist_val.size();
            std::cout << current_hist_val << std::flush;
        } else {
            std::cout << '\a' << std::flush;
        }
    }
}

void InputReader::historyDown() {
    if (in_history) {
        clearPrompt(current_hist_val.size());
        if (hist_pos + 1 < history.size()) {
            hist_pos++;
            current_hist_val = history.at((unsigned)hist_pos);
            cursor_pos = current_hist_val.size();
            std::cout << current_hist_val << std::flush;
        } else {
            in_history = false;
            cursor_pos = original_hist_cursor_pos;
            std::cout << original_hist_val << std::flush;
        }
    } else {
        std::cout << '\a' << std::flush;
    }
}

void InputReader::moveCursor(char direction) {
    switch (direction) {
        case 'A':
            historyUp();
            break;
        case 'B':
            historyDown();
            break;
        case 'C':
            moveCursorRight();
            break;
        case 'D':
            moveCursorLeft();
            break;
        default:
            break;
    }
}

void InputReader::moveCursorRight() {
    if (in_history) {
        if (cursor_pos < current_hist_val.size()) {
            cursor_pos++;
            std::cout << (char)27 << '[' << 'C' << std::flush;
        }
    } else if (in_tab_complete) {
        if (cursor_pos < current_tab_val.size()) {
            cursor_pos++;
            std::cout << (char)27 << '[' << 'C' << std::flush;
        }
    } else if (cursor_pos < output.size()) {
        cursor_pos++;
        std::cout << (char)27 << '[' << 'C' << std::flush;
    }
}

void InputReader::moveCursorLeft() {
    if (cursor_pos > 0) {
        cursor_pos--;
        std::cout << (char)27 << '[' << 'D' << std::flush;
    }
}

void InputReader::backspace() {
    if (cursor_pos > 0) {
        output.erase(output.begin() + cursor_pos - 1);
        moveCursorLeft();
        showFullText(output);
    }
}
void InputReader::showFullText(std::string text) {
    clearPrompt(text.size());
    for (unsigned long i = 0; i < text.size(); i++) {
        std::cout << text.at(i) << std::flush;
    }

    for (unsigned long i = (unsigned)cursor_pos; i < text.size(); i++) {
        std::cout << "\b" << std::flush;
    }
}

void InputReader::clearPrompt(long text_len) {
    for (unsigned long i = (unsigned)cursor_pos; i < text_len + 1; i++) {
        std::cout << (char)27 << "[C" << std::flush;
    }
    for (unsigned long i = 0; i < text_len + 1; i++) {
        std::cout << "\b \b" << std::flush;
    }
}