#include "PL0/Core/SemanticLL1Parser.hpp"
#include "PL0/Utils/Reporter.hpp"
#include <format>

namespace PL0
{
SemanticLL1Parser::SemanticLL1Parser()
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

    m_syntax.setBeginSym("S");
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
    m_syntax.calcSelectSets();

    // Semantic actions
    setAction("0", printAns, {"E"});
    setAction("1", assign, {"E'"});
    setAction("2", opposite, {"E'"});
    setAction("3", assign, {"E'"});
    setAction("4", assign, {"T"});
    setAction("5", assign, {"E''"});
    setAction("6", add, {"E''", "T"});
    setAction("7", assign, {"E''"});
    setAction("8", sub, {"E''", "T"});
    setAction("9", assign, {"E''"});
    setAction("10", assign, {"E''"});
    setAction("11", assign, {"F"});
    setAction("12", assign, {"T'"});
    setAction("13", mul, {"T'", "F"});
    setAction("14", assign, {"T'"});
    setAction("15", div, {"T'", "F"});
    setAction("16", assign, {"T'"});
    setAction("17", assign, {"T'"});
    setAction("18", assign, {"E"});
    setAction("19", assign, {"id"});
    setAction("20", assign, {"num"});

    generateTables();
    // printTable();
}

void SemanticLL1Parser::addRule(const Symbol& lhs, const std::vector<Symbol>& rhs, int indexOffset)
{
    m_rhsWithActions.push_back(rhs);

    // Remove all the actions on the right-hand side of the rule,
    // then pass the rule to the syntax processor.
    std::vector<Symbol> rhsCopy;
    for (const Symbol& sym : rhs) {
        // If a symbol begins with a digit, it is an action.
        if (!std::isdigit(sym[0])) {
            rhsCopy.push_back(sym);
        }
    }
    m_syntax.addRule(lhs, rhsCopy);

    // Add the index offset
    m_indexOffsets.push_back(indexOffset);
}

void SemanticLL1Parser::setAction(const std::string& index,
                                  const std::function<int(const std::vector<Operand>&)>& func,
                                  const std::vector<std::string>& operandNames)
{
    m_actions[index] = Action{func, {}};
    for (const std::string& name : operandNames) {
        m_actions[index].operands.push_back({name, {}});
    }
}

void SemanticLL1Parser::generateTables()
{
    const auto& rules = m_syntax.getRules();
    for (size_t i = 0; i < rules.size(); ++i) {
        const Symbol& lhs = rules[i].lhs;
        const std::set<Symbol>& selectSet = m_syntax.getSelectSet(i);

        for (const Symbol& sym : selectSet) {
            m_predictionTable[lhs][sym] = m_rhsWithActions[i];
            m_indexOffsetTable[lhs][sym] = m_indexOffsets[i];
        }
    }
}

void SemanticLL1Parser::parse(const std::vector<Token>& tokens)
{
    // Rest input
    // e.g. 3 + 5 * 2  =>  3 + 5 * 2 #
    //                      ------->
    std::vector<std::string> restInput;
    restInput.push_back("#");

    for (auto it = tokens.rbegin(); it != tokens.rend(); ++it) {
        // [NOTE] Numbers and identifiers are not translated to "num" and "id" here,
        //        because the value of numbers are needed in the semantic actions.
        //        The translation is done in the semantic actions.
        restInput.push_back(it->value);
    }

    // Analysis stack
    // Initial state (The bottom is at index 0):
    // ----------------
    // |  #  Ssyn  S
    // ----------------
    std::vector<Element> analysisStack{Element("#"), Element("S", true), Element("S")};

    try {
        while (!analysisStack.empty() && !restInput.empty()) {
            // printState(analysisStack, restInput);

            size_t atopIndex = analysisStack.size() - 1;
            const Element& atop = analysisStack.back();

            std::string rtopValue = restInput.back();
            Symbol rtopSym = translate2Symbol(rtopValue);

            if (atop.type == SymbolType::TERMINAL || atop.type == SymbolType::ENDSYM) {
                // Reporter::info("Terminal || EndSym");

                // If these two symbols do not match, throw a syntax error.
                if (atop.symbol != rtopSym) {
                    throw std::runtime_error(
                        std::format("Syntax error: The terminal symbol {} does not match "
                                    "the top of the input stack {}.",
                                    atop.symbol, rtopSym));
                }

                // If there is an identifier in the expression, throw a semantic error.
                if (atop.symbol == "id") {
                    throw std::runtime_error(
                        "Semantic error: Identifier is not allowed in the expression, "
                        "because its value is not defined.");
                }

                ///////////////////////
                //    Top matched.   //
                ///////////////////////

                if (atop.symbol == "num") {
                    // Assign the value of the number to the new top,
                    // which is an action: { F.val = num.val }.
                    // [NOTE] It is guaranteed that analysisStack.size() >= 1 in this case,
                    //        so it is safe to access analysisStack[atopIndex - 1].
                    int num = std::stoi(rtopValue);
                    Element& newAtop = analysisStack[atopIndex - 1];
                    Action& action = newAtop.action.value();
                    action.operands[0].second = num;
                }
                analysisStack.pop_back();
                restInput.pop_back();
            } else if (atop.type == SymbolType::NON_TERMINAL) {
                ////////////////////////////////////////////////////
                // Find the production rule in the prediction table.
                ////////////////////////////////////////////////////

                // Reporter::info("Non-terminal");

                // If there is no such rule, throw a syntax error.
                const auto& allRules = m_predictionTable[atop.symbol];
                if (allRules.find(rtopSym) == allRules.end()) {  // Not found
                    throw std::runtime_error(
                        std::format("Syntax error: {} is not allowed.", rtopSym));
                }
                const auto& rule = m_predictionTable[atop.symbol][rtopSym];

                ////////////////////////////////////////////////////////
                // Replace the top non-terminal symbol X with the rule.
                ////////////////////////////////////////////////////////

                // Pop the top element
                Element oldAtop = atop;
                analysisStack.pop_back();

                // Push the symbols in the rule in reverse order.
                for (auto it = rule.rbegin(); it != rule.rend(); ++it) {
                    if (!it->empty()) {  // Ignore ε
                        // If the symbol Y is a non-terminal,
                        // an additional synthesized attribute Ysyn should be pushed before Y.
                        //  ---------------------
                        //  |  ...  <-- Ysyn  Y
                        //  ---------------------
                        if (m_syntax.isNonTerminal(*it)) {
                            analysisStack.push_back(Element(*it, true));   // Ysyn
                            analysisStack.push_back(Element(*it, false));  // Y
                        } else if (std::isdigit((*it)[0])) {               // Action
                            analysisStack.push_back(Element(*it, false, m_actions[*it]));
                        } else {
                            analysisStack.push_back(Element(*it, false));
                        }
                    }
                }

                ///////////////////////////////////////////////////////////////
                // Assign the value of X to the specific position (if exists).
                ///////////////////////////////////////////////////////////////
                if (oldAtop.value.has_value()) {
                    auto indexOffset = m_indexOffsetTable[oldAtop.symbol][rtopSym];
                    Element& e = analysisStack[atopIndex + indexOffset];
                    if (e.type == SymbolType::ACTION) {  // T' -> ε
                        e.action->operands[0].second = oldAtop.value;
                    } else {
                        // [NOTE] It is guaranteed that the synthesized attribute is followed by a
                        // non-terminal symbol.
                        analysisStack[atopIndex + indexOffset + 1].value =
                            oldAtop.value;  // { E'.syn = E''.syn
                    }
                }
            } else if (atop.type == SymbolType::SYNTHESIZED) {
                // Reporter::info("Synthesized");

                //////////////////////////////////////////////////////////////////////////
                // Assign the value of the synthesized attribute to the following action.
                //////////////////////////////////////////////////////////////////////////

                // [NOTE] It is guaranteed that a synthesized attribute is followed by either an
                // action,
                //        or an end symbol.
                //        For the latter case, the synthesized attribute is not needed.
                auto it =
                    std::find_if(analysisStack.rbegin(), analysisStack.rend(),
                                 [](const Element& e) { return e.type == SymbolType::ACTION; });

                if (it != analysisStack.rend()) {
                    Action& firstAction = it->action.value();

                    // Check if the operand required by the action matches the synthesized
                    // attribute.
                    auto operand = std::find_if(
                        firstAction.operands.begin(), firstAction.operands.end(),
                        [&atop](const Operand& op) { return op.first == atop.symbol; });

                    if (operand == firstAction.operands.end()) {
                        throw std::runtime_error(
                            std::format("Semantic error: The synthesized attribute {} is "
                                        "not needed in the action {}.",
                                        atop.symbol, it->symbol));
                    }

                    // Check if the synthesized attribute is already assigned a value.
                    if (operand->second.has_value()) {
                        throw std::runtime_error(
                            std::format("Semantic error: The synthesized attribute {} is "
                                        "already assigned a value in the action {}.",
                                        atop.symbol, it->symbol));
                    }

                    // Assign the value.
                    operand->second = atop.value;
                }
                analysisStack.pop_back();

            } else if (atop.type == SymbolType::ACTION) {
                // Reporter::info("Action");

                // Check if the action is followed by a synthesized attribute or a non-terminal
                // symbol.
                auto it = std::find_if(analysisStack.rbegin(), analysisStack.rend(),
                                       [](const Element& e) {
                                           return e.type == SymbolType::SYNTHESIZED ||
                                                  e.type == SymbolType::NON_TERMINAL;
                                       });

                // Check if the required operands are all filled.
                const Action& action = atop.action.value();
                if (!action.isAllOperandFilled()) {
                    throw std::runtime_error(std::format(
                        "Semantic error: The action {} is not fully filled.", atop.symbol));
                }

                ////////////////////////////////
                // Perform the semantic action.
                ////////////////////////////////
                it->value = action.execute();
                analysisStack.pop_back();
            } else {
                throw std::runtime_error("Unknown symbol type.");
            }
        }
    } catch (const std::exception& e) {
        Reporter::error(e.what());
        return;
    }

    Reporter::info("Syntax correct.");
}

void SemanticLL1Parser::printTable()
{
    // E -- ( -> TE', id -> TE', num -> TE',
    // E' -- # -> ε, ) -> ε, + -> +TE', - -> -TE',
    // F -- ( -> (E), id -> id, num -> num,
    // T -- ( -> FT', id -> FT', num -> FT',
    // T' -- # -> ε, ) -> ε, * -> *FT', + -> ε, - -> ε, / -> /FT',

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
                                   const std::vector<std::string>& restInput)
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
    if (top.type == SymbolType::ACTION) {
        for (const auto& operand : top.action.value().operands) {
            if (operand.second.has_value()) {
                std::cout << "Operand: " << operand.first << " = " << operand.second.value()
                          << "\n";
            }
        }
    } else {
        if (top.value.has_value()) {
            std::cout << "Value: " << top.value.value() << "\n";
        }
    }

    std::cout << "Rest input: ";
    for (auto it = restInput.rbegin(); it != restInput.rend(); ++it) {
        std::cout << translate2Symbol(*it) << " ";
    }
    std::cout << "\n";
    std::cout << "--------------------------------------------------------------------------\n";
}
}  // namespace PL0