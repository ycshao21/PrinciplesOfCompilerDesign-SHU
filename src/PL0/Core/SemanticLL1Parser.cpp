#include "PL0/Core/SemanticLL1Parser.hpp"
#include "PL0/Utils/Error.hpp"
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
    /**
     * @note The syntax of arithmetic expressions:
     *  S -> E {0}                  {0} : print(E.val)
     *  E -> + E' {1}               {1} : E.syn = E'.syn
     *  E -> - E' {2}               {2} : E.syn = -E'.syn
     *  E -> E' {3}                 {3} : E.syn = E'.syn
     *  E' -> T {4} E'' {5}         {4} : E''.inh = T.val
     *                              {5} : E'.syn = E''.syn
     *  E'' -> + T {6} E''1 {7}     {6} : E''1.inh = E''.inh + T.val
     *                              {7} : E''.syn = E''1.syn
     *  E'' -> - T {8} E''1 {9}     {8} : E''1.inh = E''.inh - T.val
     *                              {9} : E''.syn = E''1.syn
     *  E'' -> ε {10}               {10} : E''.syn = E''.inh
     *  T -> F {11} T' {12}         {11} : T'.inh = F.val
     *                              {12} : T.val = T'.syn
     *  T' -> * F {13} T'1 {14}     {13} : T'1.inh = T'.inh * F.val
     *                              {14} : T'.syn = T'1.syn
     *  T' -> / F {15} T'1 {16}     {15} : if F.val == 0 then error
     *                                     T'1.inh = T'.inh / F.val
     *                              {16} : T'.syn = T'1.syn
     *  T' -> ε {17}                {17} : T'.syn = T'.inh
     *  F -> ( E {18} )             {18} : F.val = E.val
     *  F -> id {19}                {19} : error
     *  F -> num {20}               {20} : F.val = num.val
     *
     * {n} is the action symbol for semantic analysis.
     */

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

    /**
     * @note The syntax analyzer can only handle rules without actions.
     *      So, remove all the actions on the right-hand side of the rule.
     */

    std::vector<Symbol> rhsWithoutActions;
    for (const Symbol& sym : rhs) {
        /**
         * @note An action symbol begins with a digit.
         */
        if (!std::isdigit(sym[0])) {
            rhsWithoutActions.push_back(sym);
        }
    }
    m_analyzer.addRule(lhs, rhsWithoutActions);

    // Store the index offset for the non-terminal symbol.
    m_indexOffsets.push_back(indexOffset);
}

void SemanticLL1Parser::setActionFunc(const std::string& actionSym, const ActionFunc& func)
{
    if (m_actionFuncs.find(actionSym) != m_actionFuncs.end()) {
        throw std::runtime_error(std::format("Action {} already exists", actionSym));
    }
    m_actionFuncs[actionSym] = func;
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
    /**
     * @note Input stack (The bottom is at index 0):
     *   a + 5 * b  =>
     *   ----------------------------
     *   | ENDSYM  b  *  5 +  a <---
     *   ----------------------------
     */
    std::vector<std::string> inputStack{ENDSYM};

    // Push values of tokens in reverse order.
    for (auto it = tokens.rbegin(); it != tokens.rend(); ++it) {
        /**
         * @note Different from the LL1Parser,
         *      the value of each number is needed in the semantic actions.
         *      So we cannot translate them to symbols here.
         */
        inputStack.push_back(it->value);
    }

    /**
     * @note Analysis stack (The bottom is at index 0):
     *   Assume that the begin symbol is S, then the initial state is:
     *    --------------------
     *    |  ENDSYM  Ssyn  S    <---
     *    --------------------
     */
    std::vector<Element> analysisStack{Element(ENDSYM), Element(m_analyzer.getBeginSym(), true),
                                       Element(m_analyzer.getBeginSym())};

    try {
        /**
         * @note Translate the top symbol of the input stack here to avoid repeated translation.
         */
        Symbol itopSym = translate2Symbol(inputStack.back());

        while (!analysisStack.empty() && !inputStack.empty()) {
            // printState(analysisStack, inputStack);
            size_t atopIndex = analysisStack.size() - 1;
            const Element& atop = analysisStack.back();

            /**
             * @note If the top of the analysis stack is a terminal symbol or the end symbol,
             *      then it should match the top of the input stack.
             */
            if (atop.type == SymbolType::TERMINAL || atop.type == SymbolType::ENDSYM) {
                if (atop.symbol != itopSym) {  // Mismatch
                    throw SyntaxError(std::format(
                        "The terminal symbol {} does not match the top of the input stack {}.",
                        atop.symbol, itopSym));
                }

                if (atop.symbol == "id") {
                    /**
                     * @note Values of identifiers are unknown, so the result cannot be calculated.
                     */
                    throw SemanticError("Identifier is not allowed in the expression.");
                }

                if (atop.symbol == "num") {
                    /**
                     * @note Since only rule F -> num { F.val = num.val } can produce the terminal
                     * symbol "num", the next symbol of "num" must be action { F.val = num.val } So,
                     * assign the value of the number to the next symbol.
                     */

                    int num = std::stoi(inputStack.back());  // Value of the number
                    analysisStack[atopIndex - 1].values.push_back(num);
                }

                // Pop analysis stack and input stack.
                analysisStack.pop_back();
                inputStack.pop_back();

                // Update the top symbol of the input stack (if exists).
                if (!inputStack.empty()) {
                    itopSym = translate2Symbol(inputStack.back());
                }
            } else if (atop.type == SymbolType::NON_TERMINAL) {
                /**
                 * @note If the top of the analysis stack is a non-terminal symbol X,
                 *     find the production rule X -> Y1Y2...Yn in the prediction table,
                 *     and replace X with Y1Y2...Yn (in reverse order) in the analysis stack.
                 */

                const auto& items = m_predictionTable[atop.symbol];
                if (items.find(itopSym) == items.end()) {  // No such production rule
                    throw SyntaxError(std::format("{} is not allowed.", itopSym));
                }
                const auto& rhs = m_predictionTable[atop.symbol][itopSym];

                // 1) Pop X
                Element oldAtop = atop;
                analysisStack.pop_back();

                // 2) Push Yn, Yn-1, ..., Y1
                for (auto it = rhs.rbegin(); it != rhs.rend(); ++it) {
                    if (*it != EPSILON) {  // Skip ε
                        if (m_analyzer.isNonTerminal(*it)) {
                            /**
                             * @note If the symbol Y is a non-terminal, an additional synthesized
                             *      attribute Ysyn should be pushed before Y.
                             *      ---------------------
                             *      |  ...  <-- Ysyn  Y
                             *      ---------------------
                             */
                            analysisStack.push_back(Element(*it, true));  // Ysyn
                        }
                        analysisStack.push_back(Element(*it, false));
                    }
                }

                // 3) Assign the value of X to the action that needs it.
                if (oldAtop.values.size() == 1) {  // X has a value
                    /**
                     * @note It is safe to access m_indexOffsetTable[oldAtop.symbol][itopSym],
                     *      because the corresponding rule is found.
                     */
                    int indexOffset = m_indexOffsetTable[oldAtop.symbol][itopSym];
                    if (indexOffset != NULL_OFFSET) {  // The value needs to be passed
                        Element& e = analysisStack[atopIndex + indexOffset];  // Action
                        e.values.push_back(oldAtop.values[0]);
                    }
                }
            } else if (atop.type == SymbolType::SYNTHESIZED) {
                /**
                 * @note Assign the value of the synthesized attribute (if exists) to the following
                 * action For the following symbol of the synthesized attribute, there are two
                 * cases:
                 *          - Action
                 *          - ENDSYM
                 *      For the latter case, the value of the synthesized attribute is not needed.
                 */
                if (atop.values.size() == 1) {  // Has a value
                    auto it =
                        std::find_if(analysisStack.rbegin(), analysisStack.rend(),
                                     [](const Element& e) { return e.type == SymbolType::ACTION; });
                    /**
                     * @note
                     */
                    if (it != analysisStack.rend()) {  // Action found
                        it->values.push_back(atop.values[0]);
                    }
                }
                analysisStack.pop_back();
            } else if (atop.type == SymbolType::ACTION) {
                /**
                 * @note Perform the semantic action.
                 *      For the following symbol of the action, there are three cases:
                 *         - Synthesized attribute (Ssyn and Esyn in this case):
                 *            -----------------------------------
                 *            | ... Ssyn {0} Esyn {3} E'syn ....
                 *            -----------------------------------
                 *         - Non-terminal (T' in this case):
                 *            ----------------------------------
                 *            | ... Tsyn {12} T'syn T' {11} ...
                 *            ----------------------------------
                 *         - ')' (See {18} in this case):
                 *            -----------------------------
                 *            | ... {11} Fsyn ) {18} Esyn
                 *            -----------------------------
                 *      The result of the action should be assigned to the nearest synthesized
                 *      attribute or non-terminal symbol.
                 * @note There is at least one synthesized attribute in the analysis stack,
                 *      so there is no need to check the existence.
                 */

                auto it = std::find_if(analysisStack.rbegin(), analysisStack.rend(),
                                       [](const Element& e) {
                                           return e.type == SymbolType::SYNTHESIZED ||
                                                  e.type == SymbolType::NON_TERMINAL;
                                       });
                
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

    /**
     * @note It is impossible that one of the stacks is empty while the other is not.
     */
    Reporter::success("Syntax and semantics correct.");
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