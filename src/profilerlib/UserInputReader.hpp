/*
* Souffle - A Datalog Compiler
* Copyright (c) 2017, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/

#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

/*
 * A class that reads user input a char at a time allowing for tab completion and history to be implemented
 * TODO: link live reader and this class for the warning message that the live reader has finished to be
 * displayed properly
 * TODO: consider: only display history based on current input
 * - eg. history is ["rel R1", "rul N1.1", "graph R1 tot_t"] user types "rel" and presses up, will show "rel
 * R1" only
 */
class InputReader {
private:
    std::string prompt;
    std::vector<std::string> tab_completion;
    std::vector<std::string> history;
    std::string output;
    char current_char;
    long cursor_pos;
    long hist_pos;
    long tab_pos;
    bool in_tab_complete;
    bool in_history;
    std::string original_hist_val;
    std::string current_hist_val;
    std::string current_tab_val;
    std::string original_tab_val;
    std::vector<std::string> current_tab_completes;
    long original_hist_cursor_pos;

public:
    InputReader() : prompt("Input: "), in_tab_complete(false), in_history(false) {
        clearTabCompletion();
        clearHistory();
    }

    void getch();
    std::string getInput();
    void setPrompt(std::string prompt);
    void appendTabCompletion(std::vector<std::string> commands);
    void appendTabCompletion(std::string command);
    void clearTabCompletion();
    void clearHistory();
    void tabComplete();
    void addHistory(std::string hist);
    void historyUp();
    void historyDown();
    void moveCursor(char direction);
    void moveCursorRight();
    void moveCursorLeft();
    void backspace();
    void clearPrompt(long text_len);
    void showFullText(std::string text);
};
