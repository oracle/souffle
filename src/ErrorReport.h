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
 * @file ErrorReport.h
 *
 * Defines a class for error reporting.
 *
 ***********************************************************************/
#pragma once

#include <algorithm>
#include <cassert>
#include "AstSrcLocation.h"

namespace souffle {

class DiagnosticMessage {
private:
    std::string message;
    bool hasLoc;
    AstSrcLocation location;
public:
    DiagnosticMessage(std::string message, AstSrcLocation location) : message(message), hasLoc(true), location(location) { }

    DiagnosticMessage(std::string message) : message(message), hasLoc(false) { }

    const std::string &getMessage() const {
        return message;
    }

    const AstSrcLocation &getLocation() const {
        assert(hasLoc);
        return location;
    }

    bool hasLocation() const {
        return hasLoc;
    }

    void print(std::ostream &out) const {
        out << message;
        if (hasLoc) {
            out << " in " << location.extloc();
        }
        out << "\n";
    }

    friend std::ostream& operator<<(std::ostream &out, const DiagnosticMessage &diagnosticMessage) {
        diagnosticMessage.print(out);
        return out;
    }
};

class Diagnostic {
public:
    enum Type {
        ERROR,
        WARNING
    };
private:
    Type type;
    DiagnosticMessage primaryMessage;
    std::vector<DiagnosticMessage> additionalMessages;
public:

    Diagnostic(Type type, DiagnosticMessage primaryMessage, std::vector<DiagnosticMessage> additionalMessages)
            : type(type), primaryMessage(primaryMessage), additionalMessages(additionalMessages) { }

    Diagnostic(Type type, DiagnosticMessage primaryMessage) : type(type), primaryMessage(primaryMessage) { }

    Type getType() const {
        return type;
    }

    const DiagnosticMessage &getPrimaryMessage() const {
        return primaryMessage;
    }

    const std::vector<DiagnosticMessage> &getAdditionalMessages() const {
        return additionalMessages;
    }

    void print(std::ostream &out) const {
        out << (type == ERROR ? "Error: " : "Warning: ");
        out << primaryMessage;
        for (const DiagnosticMessage &additionalMessage : additionalMessages) {
            out << additionalMessage;
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const Diagnostic& diagnostic) {
         diagnostic.print(out);
         return out;
    }

    bool operator<(const Diagnostic& other) const {
        if (primaryMessage.hasLocation() && !other.primaryMessage.hasLocation()) return true;
        if (other.primaryMessage.hasLocation() && !primaryMessage.hasLocation()) return false;

        if (primaryMessage.hasLocation() && other.primaryMessage.hasLocation()) {
            if (primaryMessage.getLocation() < other.primaryMessage.getLocation()) return true;
            if (other.primaryMessage.getLocation() < primaryMessage.getLocation()) return false;
        }

        if (type == ERROR && other.getType() == WARNING) return true;
        if (other.getType() == ERROR && type == WARNING) return false;

        if (primaryMessage.getMessage() < other.primaryMessage.getMessage()) return true;
        if (other.primaryMessage.getMessage() < primaryMessage.getMessage()) return false;

        return false;
    }
};

class ErrorReport {
    std::set<Diagnostic> diagnostics;
public:

    ErrorReport() { }

    ErrorReport(const ErrorReport &other) : diagnostics(other.diagnostics) { }

    unsigned getNumErrors() const {
        return std::count_if(diagnostics.begin(), diagnostics.end(), [](Diagnostic d) -> bool { return d.getType() == Diagnostic::ERROR; });
    }

    unsigned getNumWarnings() const {
        return std::count_if(diagnostics.begin(), diagnostics.end(), [](Diagnostic d) -> bool { return d.getType() == Diagnostic::WARNING; });
    }

    unsigned getNumIssues() const {
        return diagnostics.size();
    }

    /** Adds an error with the given message and location */
    void addError(const std::string &message, AstSrcLocation location) {
        diagnostics.insert(Diagnostic(Diagnostic::ERROR, DiagnosticMessage(message, location)));
    }

    /** Adds a warning with the given message and location */
    void addWarning(const std::string &message, AstSrcLocation location) {
        diagnostics.insert(Diagnostic(Diagnostic::WARNING, DiagnosticMessage(message, location)));
    }

    void addDiagnostic(Diagnostic diagnostic) {
        diagnostics.insert(diagnostic);
    }

    void print(std::ostream &out) const {
        for (const Diagnostic &diagnostic : diagnostics) {
            out << diagnostic;
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const ErrorReport& report) {
        report.print(out);
        return out;
    }

};

} // end of namespace souffle

