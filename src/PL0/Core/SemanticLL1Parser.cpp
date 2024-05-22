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
     * @note 算术表达式的文法：
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
     * {n} 是语义分析中的动作。
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
     * @note 分析器只能处理没有动作的规则，所以先删除规则右侧的所有动作。
    */

    std::vector<Symbol> rhsWithoutActions;
    for (const Symbol& sym : rhs) {
        /**
         * @note 动作符号以数字开头
        */
        if (!std::isdigit(sym[0])) {
            rhsWithoutActions.push_back(sym);
        }
    }
    m_analyzer.addRule(lhs, rhsWithoutActions);

    // 将非终结符的索引偏移量存储起来。
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
     * @note 剩余输入串（栈底下标为 0）：
     *   a + 5 * b  =>
     *   ----------------------------
     *   | ENDSYM  b  *  5 +  a <---
     *   ----------------------------
     */
    std::vector<std::string> inputStack{ENDSYM};

    // 将所有词逆序推入栈中。
    for (auto it = tokens.rbegin(); it != tokens.rend(); ++it) {
        /**
         * @note 与 LL1Parser 不同，数字的值在语义分析时会用到，所以不能在这里将它们转换为符号。
        */
        inputStack.push_back(it->value);
    }

    /**
     * @note 分析栈（栈底下标为 0）：
     *   假设开始符号为 S，则初始状态为：
     *    --------------------
     *    |  ENDSYM  Ssyn  S    <---
     *    --------------------
     */
    std::vector<Element> analysisStack{Element(ENDSYM), Element(m_analyzer.getBeginSym(), true),
                                       Element(m_analyzer.getBeginSym())};

    try {
        /**
         * @note 将输入栈的顶部符号转换为符号，避免下面的循环中重复转换。
        */
        Symbol itopSym = translate2Symbol(inputStack.back());

        while (!analysisStack.empty() && !inputStack.empty()) {
            // printState(analysisStack, inputStack);
            size_t atopIndex = analysisStack.size() - 1;
            const Element& atop = analysisStack.back();

            /**
             * @note 如果分析栈顶部是终结符或结束符号，则正常情况下应该与输入栈顶部匹配。
            */
            if (atop.type == SymbolType::TERMINAL || atop.type == SymbolType::ENDSYM) {
                if (atop.symbol != itopSym) {
                    throw SyntaxError(std::format(
                        "The terminal symbol {} does not match the top of the input stack {}.",
                        atop.symbol, itopSym));
                }

                if (atop.symbol == "id") {
                    /**
                     * @note 由于不知道标识符的值，所以无法计算结果。
                     */
                    throw SemanticError("Identifier is not allowed in the expression.");
                }

                if (atop.symbol == "num") {
                    /**
                     * @note 只有规则 F -> num { F.val = num.val } 可以产生终结符 "num"，所以 "num"
                     * 的下一个符号一定是动作 { F.val = num.val }，因此将数字的值赋给下一个符号。
                     */

                    int num = std::stoi(inputStack.back());  // Value of the number
                    analysisStack[atopIndex - 1].values.push_back(num);
                }

                // 分析栈顶部和输入栈顶部匹配，因此将它们弹出。
                analysisStack.pop_back();
                inputStack.pop_back();

                // 如果输入栈不为空，则更新输入栈的顶部符号。
                if (!inputStack.empty()) {
                    itopSym = translate2Symbol(inputStack.back());
                }
            } else if (atop.type == SymbolType::NON_TERMINAL) {
                /**
                 * @note 如果分析栈顶部是非终结符 X，
                 *      在预测分析表中查找产生式 X -> Y1Y2...Yn，
                 *      并将 X 出栈，将 Y1Y2...Yn 中的非空符号逆序推入分析栈。
                */

                const auto& items = m_predictionTable[atop.symbol];
                if (items.find(itopSym) == items.end()) {  // 规则不存在
                    throw SyntaxError(std::format("{} is not allowed.", itopSym));
                }
                const auto& rhs = m_predictionTable[atop.symbol][itopSym];

                // 1) X 出栈
                Element oldAtop = atop;
                analysisStack.pop_back();

                // 2) 将 Y1Y2...Yn 中的非空符号逆序推入分析栈。
                for (auto it = rhs.rbegin(); it != rhs.rend(); ++it) {
                    if (*it != EPSILON) {  // 跳过 ε
                        if (m_analyzer.isNonTerminal(*it)) {
                            /**
                             * @note 如果符号 Y 是一个非终结符，则在 Y 之前推入一个额外的综合属性元素 Ysyn。
                             *      ---------------------
                             *      |  ...  <-- Ysyn  Y
                             *      ---------------------
                             */
                            analysisStack.push_back(Element(*it, true));  // Ysyn
                        }
                        analysisStack.push_back(Element(*it, false));
                    }
                }

                // 3) 将 X 的值传递给需要的动作。
                if (oldAtop.values.size() == 1) {  // X 有一个值
                    /**
                     * @note m_indexOffsetTable[oldAtop.symbol][itopSym] 可以安全地访问，因为找到了相应索引的规则。
                     */
                    int indexOffset = m_indexOffsetTable[oldAtop.symbol][itopSym];
                    if (indexOffset != NULL_OFFSET) {  // The value needs to be passed
                        Element& e = analysisStack[atopIndex + indexOffset];  // Action
                        e.values.push_back(oldAtop.values[0]);
                    }
                }
            } else if (atop.type == SymbolType::SYNTHESIZED) {
                /**
                 * @note 将综合属性的值（如果存在）赋给后面的动作。
                 *     对于综合属性的后面符号，有两种情况：
                 *          - 动作
                 *          - 结束符
                 *      第二种情况下，可以直接丢弃综合属性的值。
                 */
                if (atop.values.size() == 1) {  // 有一个值
                    auto it =
                        std::find_if(analysisStack.rbegin(), analysisStack.rend(),
                                     [](const Element& e) { return e.type == SymbolType::ACTION; });

                    if (it != analysisStack.rend()) {  // 找到了动作
                        it->values.push_back(atop.values[0]);
                    }
                }
                analysisStack.pop_back();
            } else if (atop.type == SymbolType::ACTION) {
                /**
                 * @note 执行语义动作。
                 *      对于动作的后面符号，有三种情况：
                 *         - 综合属性
                 *            -----------------------------------
                 *            | ... Ssyn {0} Esyn {3} E'syn ....
                 *            -----------------------------------
                 *         - 非终结符
                 *            ----------------------------------
                 *            | ... Tsyn {12} T'syn T' {11} ...
                 *            ----------------------------------
                 *         - ')'
                 *            -----------------------------
                 *            | ... {11} Fsyn ) {18} Esyn
                 *            -----------------------------
                 *      动作的结果应该赋给最近的综合属性或非终结符。
                 * @note 分析栈中至少有一个综合属性，所以不需要检查是否存在。
                 */

                auto it = std::find_if(analysisStack.rbegin(), analysisStack.rend(),
                                       [](const Element& e) {
                                           return e.type == SymbolType::SYNTHESIZED ||
                                                  e.type == SymbolType::NON_TERMINAL;
                                       });
                
                // 执行语义动作。
                ActionFunc& action = m_actionFuncs[atop.symbol];
                int result = action(atop.values);
                analysisStack.pop_back();

                // 将结果赋给最近的综合属性或非终结符。
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
     * @note 如果分析栈和输入栈都为空，则语法正确。
     *      不可能存在其中一个栈为空而另一个不为空的情况。
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