#include "PL0/Core/SemanticLL1Parser.hpp"
#include "PL0/Utils/Exception.hpp"
#include "PL0/Utils/Reporter.hpp"
#include <format>

namespace PL0
{
SemanticLL1Parser::SemanticLL1Parser()
{
    initSyntax();
}

void SemanticLL1Parser::initSyntax()
{
    // Arithmetic expression:
    //   S -> E {0}                 {0} : print(E.val)
    //   E -> + E' {1}              {1} : E.syn = E'.syn
    //   E -> - E' {2}              {2} : E.syn = -E'.syn
    //   E -> E' {3}                {3} : E.syn = E'.syn
    //   E' -> T {4} E'' {5}        {4} : E''.inh = T.val               {5} : E'.syn = E''.syn
    //   E'' -> + T {6} E''1 {7}    {6} : E''.inh = E''.inh + T.val     {7} : E''.syn = E''1.syn
    //   E'' -> - T {8} E''1 {9}    {8} : E''.inh = E''.inh - T.val     {9} : E''.syn = E''1.syn
    //   E'' -> ε {10}              {10} : E''.syn = E''.inh
    //   T -> F {11} T' {12}        {11} : T'.inh = F.val               {12} : T.val = T'.syn
    //   T' -> * F {13} T'1 {14}    {13} : T'1.inh = T'.inh * F.val     {14} : T'.syn = T'1.syn
    //   T' -> / F {15} T'1 {16}    {15} : if F.val == 0 then error; T'1.inh = T'.inh / F.val
    //                              {16} : T'.syn = T'1.syn
    //   T' -> ε {17}               {17} : T'.syn = T'.inh
    //   F -> ( E {18} )            {18} : F.val = E.val
    //   F -> id {19}               {19} : error
    //   F -> num {20}              {20} : F.val = num.val
    //
    // where E is for expression, T is for term, F is for factor.

    m_analyzer.setBeginSym("S");
    addRule("S", {"E", "0"});
    addRule("E", {"+", "E'", "1"});
    addRule("E", {"-", "E'", "2"});
    addRule("E", {"E'", "3"});
    addRule("E'", {"T", "4", "E''", "5"});
    addRule("E''", {"+", "T", "6", "E''", "7"}, 3);
    addRule("E''", {"-", "T", "8", "E''", "9"}, 3);
    addRule("E''", {EPSILON, "10"}, 0);
    addRule("T", {"F", "11", "T'", "12"});
    addRule("T'", {"*", "F", "13", "T'", "14"}, 3);
    addRule("T'", {"/", "F", "15", "T'", "16"}, 3);
    addRule("T'", {EPSILON, "17"}, 0);
    addRule("F", {"(", "E", "18", ")"});
    addRule("F", {"id", "19"});
    addRule("F", {"num", "20"});

    m_analyzer.calcSelectSets();
    generateTables();

    // Semantic actions
    setActionFunc("0", Action::print);
    setActionFunc("1", Action::assign);
    setActionFunc("2", Action::opposite);
    setActionFunc("3", Action::assign);
    setActionFunc("4", Action::assign);
    setActionFunc("5", Action::assign);
    setActionFunc("6", Action::add);
    setActionFunc("7", Action::assign);
    setActionFunc("8", Action::sub);
    setActionFunc("9", Action::assign);
    setActionFunc("10", Action::assign);
    setActionFunc("11", Action::assign);
    setActionFunc("12", Action::assign);
    setActionFunc("13", Action::mul);
    setActionFunc("14", Action::assign);
    setActionFunc("15", Action::div);
    setActionFunc("16", Action::assign);
    setActionFunc("17", Action::assign);
    setActionFunc("18", Action::assign);
    setActionFunc("19", Action::assign);
    setActionFunc("20", Action::assign);
}

void SemanticLL1Parser::addRule(const Symbol& lhs, const std::vector<Symbol>& rhs, int indexOffset)
{
    m_rhsWithActions.push_back(rhs);

    // Remove all the actions on the right-hand side of the rule,
    // then pass the rule to the analyzer.
    std::vector<Symbol> rhsCopy;
    for (const Symbol& sym : rhs) {
        // If a symbol begins with a digit, it is an action.
        if (!std::isdigit(sym[0])) {
            rhsCopy.push_back(sym);
        }
    }
    m_analyzer.addRule(lhs, rhsCopy);

    // Add the index offset
    m_indexOffsets.push_back(indexOffset);
}

void SemanticLL1Parser::setActionFunc(const std::string& index, const ActionFunc& func)
{
    if (m_actionFuncs.find(index) != m_actionFuncs.end()) {
        throw std::runtime_error(std::format("Action {} already exists", index));
    }
    m_actionFuncs[index] = func;
}

void SemanticLL1Parser::generateTables()
{
    const auto& rules = m_analyzer.getRules();
    for (size_t i = 0; i < rules.size(); ++i) {
        const Symbol& lhs = rules[i].lhs;
        const std::set<Symbol>& selectSet = m_analyzer.getSelectSet(i);

        for (const Symbol& sym : selectSet) {
            m_predictionTable[lhs][sym] = m_rhsWithActions[i];
            m_indexOffsetTable[lhs][sym] = m_indexOffsets[i];
        }
    }
}

void SemanticLL1Parser::parse(const std::vector<Token>& tokens)
{
    // Input stack
    // e.g. 3 + 5 * 2  =>  3 + 5 * 2 #
    //                      ------->
    std::vector<std::string> inputStack{ENDSYM};
    for (auto it = tokens.rbegin(); it != tokens.rend(); ++it) {
        // [NOTE] Numbers and identifiers are not translated to "num" and "id" here,
        //        because the value of numbers are needed in the semantic actions.
        //        The translation is done in the semantic actions.
        inputStack.push_back(it->value);
    }

    // Analysis stack
    // Initial state (The bottom is at index 0):
    // ----------------
    // |  #  Ssyn  S      <---
    // ----------------
    std::vector<Element> analysisStack{Element(ENDSYM), Element(m_analyzer.getBeginSym(), true),
                                       Element(m_analyzer.getBeginSym())};

    try {
        // Translate the value of the input token to a symbol.
        Symbol rtopSym = translate2Symbol(inputStack.back());

        while (!analysisStack.empty() && !inputStack.empty()) {
            // printState(analysisStack, inputStack);
            size_t atopIndex = analysisStack.size() - 1;
            const Element& atop = analysisStack.back();

            if (atop.type == SymbolType::TERMINAL || atop.type == SymbolType::ENDSYM) {
                // If these two symbols do not match, throw a syntax error.
                if (atop.symbol != rtopSym) {
                    throw SyntaxError(std::format(
                        "The terminal symbol {} does not match the top of the input stack {}.",
                        atop.symbol, rtopSym));
                }

                // If the top is an identifier, throw a semantic error.
                if (atop.symbol == "id") {
                    throw SemanticError("Identifier is not allowed in the expression.");
                }

                if (atop.symbol == "num") {
                    // Fetch the value of the number.
                    int num = std::stoi(inputStack.back());

                    // Assign the value of the number to the new top.
                    // [NOTE] The new top is certainly an action: { F.val = num.val },
                    //        because only F -> num can produce the terminal symbol "num".
                    //        And it is safe to access analysisStack[atopIndex - 1].
                    Element& newAtop = analysisStack[atopIndex - 1];
                    newAtop.values.push_back(num);
                }

                // Pop analysis stack and input stack.
                analysisStack.pop_back();
                inputStack.pop_back();

                // Update the top symbol of the input stack.
                if (!inputStack.empty()) {
                    rtopSym = translate2Symbol(inputStack.back());
                }
            } else if (atop.type == SymbolType::NON_TERMINAL) {
                //////////////////////////////////////////////////////////////////////
                // Replace the top non-terminal symbol X with the corresponding rule.
                //////////////////////////////////////////////////////////////////////

                // If there is no such rule, throw a syntax error.
                const auto& allRules = m_predictionTable[atop.symbol];
                if (allRules.find(rtopSym) == allRules.end()) {  // Not found
                    throw SyntaxError(std::format("{} is not allowed.", rtopSym));
                }
                const auto& rule = m_predictionTable[atop.symbol][rtopSym];

                // 1) Pop X
                Element oldAtop = atop;
                analysisStack.pop_back();

                // 2) Push the symbols of the rule in reverse order.
                for (auto it = rule.rbegin(); it != rule.rend(); ++it) {
                    if (*it != EPSILON) {  // Ignore ε
                        // If the symbol Y is a non-terminal,
                        // an additional synthesized attribute Ysyn should be pushed before Y.
                        //  ---------------------
                        //  |  ...  <-- Ysyn  Y
                        //  ---------------------
                        if (m_analyzer.isNonTerminal(*it)) {
                            analysisStack.push_back(Element(*it, true));   // Ysyn
                            analysisStack.push_back(Element(*it, false));  // Y
                        } else {
                            analysisStack.push_back(Element(*it, false));
                        }
                    }
                }

                // 3) Assign the value of X to the ACTION that needs it.
                // [NOTE] It is guaranteed that non-terminal symbols have at most one value.
                if (oldAtop.values.size() == 1) {
                    // [NOTE] The index of the ACTION is stored in m_indexOffsetTable.
                    //        It is safe to access m_indexOffsetTable[oldAtop.symbol][rtopSym],
                    //        because the rule is found in the prediction table.
                    int indexOffset = m_indexOffsetTable[oldAtop.symbol][rtopSym];
                    if (indexOffset != NO_VALUE) {
                        Element& e = analysisStack[atopIndex + indexOffset];
                        e.values.push_back(oldAtop.values[0]);
                    }
                }
            } else if (atop.type == SymbolType::SYNTHESIZED) {
                ////////////////////////////////////////////////////////////////////////////////////
                // Assign the value of the synthesized attribute to the following action and pop it.
                ////////////////////////////////////////////////////////////////////////////////////

                // [NOTE] It is guaranteed that a synthesized attribute has at most one value.
                // For the following symbol of the synthesized attribute, there are two cases:
                // - Action (Esyn in this case):
                //     ------------------------
                //     | ... Ssyn {0} Esyn ...
                //     ------------------------
                // - End symbol (Ssyn in this case):
                //     ---------------------
                //     | # Ssyn {0} ...
                //     ---------------------
                // For the latter case, the value of the synthesized attribute is not needed.

                if (atop.values.size() == 1) {
                    auto it =
                        std::find_if(analysisStack.rbegin(), analysisStack.rend(),
                                     [](const Element& e) { return e.type == SymbolType::ACTION; });

                    if (it != analysisStack.rend()) {
                        it->values.push_back(atop.values[0]);
                    }
                }
                analysisStack.pop_back();
            } else if (atop.type == SymbolType::ACTION) {
                // For the following symbol of the action, there are three cases:
                // - Synthesized attribute ({0} and {3} in this case):
                //      -----------------------------------
                //      | ... Ssyn {0} Esyn {3} E'syn ....
                //      -----------------------------------
                // - Non-terminal symbol ({11} in this case):
                //      ----------------------------------
                //      | ... Tsyn {12} T'syn T' {11} ...
                //      ----------------------------------
                // - ')' ({18} in this case):
                //      -----------------------------
                //      | ... {11} Fsyn ) {18} Esyn
                //      -----------------------------

                // Find the nearest synthesized attribute or non-terminal symbol.
                auto it = std::find_if(analysisStack.rbegin(), analysisStack.rend(),
                                       [](const Element& e) {
                                           return e.type == SymbolType::SYNTHESIZED ||
                                                  e.type == SymbolType::NON_TERMINAL;
                                       });
                if (it == analysisStack.rend()) {
                    throw SyntaxError("Missing synthesized attribute or non-terminal "
                                      "symbol after the action.");
                }

                // Perform the semantic action.
                ActionFunc& action = m_actionFuncs[atop.symbol];
                int result = action(atop.values);
                analysisStack.pop_back();

                // Assign the result to the symbol.
                it->values.push_back(result);
            } else {
                throw SyntaxError("Unknown symbol type.");
            }
        }
    } catch (const SyntaxError& e) {
        Reporter::error(e.what());
        return;
    } catch (const SemanticError& e) {
        Reporter::error(e.what());
        return;
    }

    Reporter::success("Syntax correct.");
}

void SemanticLL1Parser::printPredictionTable()
{
    for (const auto& [lhs, item] : m_predictionTable) {
        std::cout << lhs << " -- ";
        for (const auto& [selectSym, rhs] : item) {
            std::cout << selectSym << " -> ";
            for (const auto& s : rhs) {
                if (s == EPSILON) {
                    std::cout << "ε";
                } else if (std::isdigit(s[0])) {
                    std::cout << "{" << s << "}";
                } else {
                    std::cout << s;
                }
            }
            std::cout << ", ";
        }
        std::cout << "\n";
    }
}

void SemanticLL1Parser::printState(const std::vector<Element>& analysisStack,
                                   const std::vector<std::string>& inputStack)
{
    std::cout << "Analysis stack: ";
    for (const auto& sym : analysisStack) {
        if (sym.type == SymbolType::SYNTHESIZED) {
            std::cout << sym.symbol << "syn ";
        } else if (sym.type == SymbolType::ACTION) {
            std::cout << "{" << sym.symbol << "} ";
        } else {
            std::cout << sym.symbol << " ";
        }
    }
    std::cout << "\n";

    auto& top = analysisStack.back();
    for (const auto& v : top.values) {
        std::cout << "Value: " << v << "\n";
    }

    std::cout << "Input stack: ";
    for (auto it = inputStack.rbegin(); it != inputStack.rend(); ++it) {
        std::cout << translate2Symbol(*it) << " ";
    }
    std::cout << "\n";
    std::cout << "--------------------------------------------------------------------------\n";
}
}  // namespace PL0