/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All Rights reserved
 * 
 * The Universal Permissive License (UPL), Version 1.0
 * 
 * Subject to the condition set forth below, permission is hereby granted to any person obtaining a copy of this software,
 * associated documentation and/or data (collectively the "Software"), free of charge and under any and all copyright rights in the 
 * Software, and any and all patent rights owned or freely licensable by each licensor hereunder covering either (i) the unmodified 
 * Software as contributed to or provided by such licensor, or (ii) the Larger Works (as defined below), to deal in both
 * 
 * (a) the Software, and
 * (b) any piece of software and/or hardware listed in the lrgrwrks.txt file if one is included with the Software (each a “Larger
 * Work” to which the Software is contributed by such licensors),
 * 
 * without restriction, including without limitation the rights to copy, create derivative works of, display, perform, and 
 * distribute the Software and make, use, sell, offer for sale, import, export, have made, and have sold the Software and the 
 * Larger Work(s), and to sublicense the foregoing rights on either these or other terms.
 * 
 * This license is subject to the following condition:
 * The above copyright notice and either this complete permission notice or at a minimum a reference to the UPL must be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/************************************************************************
 *
 * @file DebugReport.h
 *
 * Defines classes for creating HTML reports of debugging information.
 *
 ***********************************************************************/
#pragma once

#include <memory>
#include <vector>
#include <sstream>
#include <ostream>

#include "AstTransformer.h"

namespace souffle {

class AstTranslationUnit;

/**
 * Class representing a section of a HTML report.
 * Consists of a unique identifier, a title, a number of subsections,
 * and the HTML code for the body of the section.
 */
class DebugReportSection {
private:
    std::string id;
    std::string title;
    std::vector<DebugReportSection> subsections;
    std::string body;
public:
    DebugReportSection(std::string id, std::string title, std::vector<DebugReportSection> subsections, std::string body) :
            id(id), title(title), subsections(subsections), body(body) { }

    /**
     * Outputs the HTML code for the index to the given stream,
     * consisting of a link to the section body followed by a list of
     * the indices for each subsection.
     */
    void printIndex(std::ostream &out) const;

    /**
     * Outputs the HTML code for the title header to the given stream.
     */
    void printTitle(std::ostream &out) const;

    /**
     * Outputs the HTML code for the content of the section to the given
     * stream, consisting of the title header, the body text, followed
     * by the content for each subsection.
     */
    void printContent(std::ostream &out) const;

    bool hasSubsections() const {
        return !subsections.empty();
    }

};

/**
 * Class representing a HTML report, consisting of a list of sections.
 */
class DebugReport {
private:
    std::vector<DebugReportSection> sections;
public:
    bool empty() const {
        return sections.empty();
    }

    void addSection(const DebugReportSection &section) {
        sections.push_back(section);
    }

    /**
     * Outputs a complete HTML document to the given stream,
     * consisting of an index of all of the sections of the report,
     * followed by the content of each section.
     */
    void print(std::ostream &out) const;


    friend std::ostream& operator<<(std::ostream& out, const DebugReport& report) {
        report.print(out);
        return out;
    }
};

/**
 * Transformation pass which wraps another transformation pass and generates
 * a debug report section for the stage after applying the wrapped transformer,
 * and adds it to the translation unit's debug report.
 */
class DebugReporter : public AstTransformer {
private:
    std::unique_ptr<AstTransformer> wrappedTransformer;

    virtual bool transform(AstTranslationUnit &translationUnit);
public:
    DebugReporter(std::unique_ptr<AstTransformer> wrappedTransformer) : wrappedTransformer(std::move(wrappedTransformer)) { }

    virtual std::string getName() const {
        return "DebugReporter";
    }

    /**
     * Generate a debug report section for the current state of the given translation unit
     * with the given id and title, and add the section to the translation unit's debug report.
     * @param translationUnit translation unit to generate and add debug report section
     * @param id the unique id of the generated section
     * @param title the text to display as the heading of the section
     */
    static void generateDebugReport(AstTranslationUnit &translationUnit, std::string id, std::string title);

    /**
     * Generate a debug report section for code (preserving formatting), with the given id and title.
     */
    static DebugReportSection getCodeSection(std::string id, std::string title, std::string code);

    /**
     * Generated a debug report section for a dot graph specification, with the given id and title.
     */
    static DebugReportSection getDotGraphSection(std::string id, std::string title, std::string dotSpec);
};

} // end of namespace souffle

