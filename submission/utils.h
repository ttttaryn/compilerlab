#include <iostream>
#include <cctype>
#include <fstream>
#include <vector>
#include <map>
#include <stack>
#include <string>
using namespace std;

/**
 * description: MIPS汇编代码生成，将MIPS汇编代码追加写到target字符串；这里使用静态方法，不需要实例化对象；
 */
struct MIPS
{
    static void sw(string &target, string reg, int offset, string base)
    {
        target += "\tsw $" + reg + ", " + to_string(offset) + "($" + base + ")\n";
    }

    static void lw(string &target, string reg, int offset, string base)
    {
        target += "\tlw $" + reg + ", " + to_string(offset) + "($" + base + ")\n";
    }

    static void add(string &target, string reg1, string reg2, string reg3)
    {
        target += "\tadd $" + reg1 + ", $" + reg2 + ", $" + reg3 + "\n";
    }

    static void sub(string &target, string reg1, string reg2, string reg3)
    {
        target += "\tsub $" + reg1 + ", $" + reg2 + ", $" + reg3 + "\n";
    }

    static void mul(string &target, string reg1, string reg2, string reg3)
    {
        target += "\tmul $" + reg1 + ", $" + reg2 + ", $" + reg3 + "\n";
    }

    static void div(string &target, string reg1, string reg2, string reg3)
    {
        target += "\tdiv $" + reg1 + ", $" + reg2 + ", $" + reg3 + "\n";
    }

    static void li(string &target, string reg, int value)
    {
        target += "\tli $" + reg + ", " + to_string(value) + "\n";
    }

    static void mfhi(string &target, string reg)
    {
        target += "\tmfhi $" + reg + "\n";
    }

    static void slt(string &target, string reg1, string reg2, string reg3)
    {
        target += "\tslt $" + reg1 + ", $" + reg2 + ", $" + reg3 + "\n";
    }

    static void xori(string &target, string reg1, string reg2, int value)
    {
        target += "\txori $" + reg1 + ", $" + reg2 + ", " + to_string(value) + "\n";
    }

    static void seq(string &target, string reg1, string reg2, string reg3)
    {
        target += "\tseq $" + reg1 + ", $" + reg2 + ", $" + reg3 + "\n";
    }

    static void sne(string &target, string reg1, string reg2, string reg3)
    {
        target += "\tsne $" + reg1 + ", $" + reg2 + ", $" + reg3 + "\n";
    }

    static void or_(string &target, string reg1, string reg2, string reg3)
    {
        target += "\tor $" + reg1 + ", $" + reg2 + ", $" + reg3 + "\n";
    }

    static void and_(string &target, string reg1, string reg2, string reg3)
    {
        target += "\tand $" + reg1 + ", $" + reg2 + ", $" + reg3 + "\n";
    }

    static void xor_(string &target, string reg1, string reg2, string reg3)
    {
        target += "\txor $" + reg1 + ", $" + reg2 + ", $" + reg3 + "\n";
    }

    static void addi(string &target, string reg1, string reg2, int value)
    {
        target += "\taddi $" + reg1 + ", $" + reg2 + ", " + to_string(value) + "\n";
    }

    static void move(string &target, string reg1, string reg2)
    {
        target += "\tmove $" + reg1 + ", $" + reg2 + "\n";
    }

    static void syscall(string &target)
    {
        target += "\tsyscall\n";
    }

    static void la(string &target, string reg, string label)
    {
        target += "\tla $" + reg + ", " + label + "\n";
    }
};


struct Token
{
    string type; // Operator, Identifier, Constant, Keyword, Delimiter
    string value;
};


struct Parser
{
    vector<Token> tokens;           // 词法分析得到的Token序列，用于语法分析
    map<string, int> variables_loc; // 变量在栈中的位置，存储相对于fp的偏移量，一般为负数（单位：字节）
    int stack_alloc_index = 0;      // 当前已使用栈指针的偏移量
    string target;                  // 生成的MIPS汇编代码字符串
    int index = 0;                  // 当前解析到的Token位置

    Parser(vector<Token> tokens) : tokens(tokens) {}

    /**
     * description: 解析一句话，即一个表达式语句或赋值语句，生成MIPS汇编代码追加到target字符串
     */
    void parseSentence()
    {
        // 解析int类型变量定义
        if (tokens[index].type == "Keyword")
        {
            if (tokens[index].value == "int")
            {
                index++;
                Token identifier = tokens[index];
                if (tokens[index].type == "Identifier")
                {
                    index++;
                    if (tokens[index].type == "Delimiter" && tokens[index].value == ";")
                    { // 如果是仅定义没有赋值的变量，则初始化为0
                        index++;
                        assign(identifier, "zero");
                        return;
                    }
                    else if (tokens[index].type == "Operator" && tokens[index].value == "=")
                    { // 如果是定义并赋值的变量，则等式右边的表达式计算结果存入变量
                        index++;
                        string reg = parseExpression();
                        if (tokens[index].type == "Delimiter" && tokens[index].value == ";")
                        {
                            index++;
                            assign(identifier, reg);
                            return;
                        }
                    }
                    else
                    {
                        cout << "Syntax Error, expect ; or =" << endl;
                        return;
                    }
                }
                else
                {
                    cout << "Syntax Error, expect Identifier" << endl;
                    return;
                }
            }

            // 解析return语句
            if (tokens[index].value == "return")
            {
                index++;
                string reg = parseExpression("v0", "v1");
                return;
            }
        }

        if (tokens[index].type == "Identifier")
        {
            // 解析赋值语句
            if (tokens[index + 1].type == "Operator" && tokens[index + 1].value == "=")
            {
                Token identifier = tokens[index];
                index += 2;
                string reg = parseExpression();
                if (tokens[index].type == "Delimiter" && tokens[index].value == ";")
                {
                    index++;
                    assign(identifier, reg);
                }
                else
                {
                    cout << "Syntax Error, expect ;" << endl;
                }
                return;
            }

            // 解析println_int(int a)函数调用
            if (tokens[index].type == "Identifier" && tokens[index].value == "println_int")
            {
                index++;
                if (tokens[index].type == "Operator" && tokens[index].value == "(")
                {
                    string reg = parseExpression();
                    // 此时reg中存储的是println_int函数的参数值，index指向)后的分号
                    Println(reg);
                    index++;
                }
                else
                {
                    cout << "Syntax Error, expect ( after println_int" << endl;
                }
                return;
            }
        }

        index++;
    }

    /**
     * description: 循环调用parseSentence()，直到遇到EOF，最终生成的MIPS汇编代码存入target字符串
     */
    void parse()
    {
        deleteMain(); // 删除main函数外的代码，仅保留main函数内的代码
        index = 0;
        target = "";

        while (index < tokens.size())
        {
            parseSentence();
        }    
    }

    /**
     * description: 获取变量在栈中的位置，如果不存在则分配位置
     */
    int getIdentifierLoc(string identifier)
    {
        if (variables_loc.find(identifier) == variables_loc.end())
        {
            stack_alloc_index -= 4;
            variables_loc[identifier] = stack_alloc_index;
        }
        return variables_loc[identifier];
    }

    /**
     * description: 获取常量的值（将字符串转为整数）
     */
    int getConstantValue(string constant)
    {
        return stoi(constant);
    }

    /**
     * description: 解析表达式，生成MIPS汇编代码，返回最终结果存储的寄存器名
     * param: t0, t1, t2 - 临时寄存器名
     * return: 最终结果存储的寄存器名
     * note: 由于MIPS汇编中最多同时一个临时变量，因此这里直接使用寄存器t0存储，每次二元运算将操作数载入t1和t2，结果存入t0
     */
    string parseExpression(string t0 = "t0", string t1 = "t1", string t2 = "t2")
    {
        // 使用栈结构解析表达式，包含+、-、*、/、%、(、)、<、>、<=、>=、==、!=、|、&、^，均为二元运算符，优先级参考C语言运算符优先级

        // 运算符优先级表
        // in-stack priority, in-coming priority
        // 优先级越高，数字越大；相同计算用操作符栈顶元素优先级高
        static map<string, int> isp, icp;

        // 由于计算中最多同时一个临时变量，因此这里直接使用寄存器t0存储
        static const Token tempOperand = {"Register", t0};

        icp[";"] = 0;
        isp[";"] = 0; // 由于;是表达式结束符，因此优先级最低

        icp[")"] = 1;
        isp[")"] = 16;
        isp["("] = 1;
        icp["("] = 16;

        icp["|"] = 2;
        isp["|"] = 3;
        icp["&"] = 4;
        isp["&"] = 5;
        icp["^"] = 6;
        isp["^"] = 7;

        icp["=="] = 8;
        isp["=="] = 9;
        icp["!="] = 8;
        isp["!="] = 9;

        icp["<"] = 10;
        isp["<"] = 11;
        icp[">"] = 10;
        isp[">"] = 11;
        icp["<="] = 10;
        isp["<="] = 11;
        icp[">="] = 10;
        isp[">="] = 11;

        icp["+"] = 12;
        isp["+"] = 13;
        icp["-"] = 12;
        isp["-"] = 13;

        icp["*"] = 14;
        isp["*"] = 15;
        icp["/"] = 14;
        isp["/"] = 15;
        icp["%"] = 14;
        isp["%"] = 15;

        stack<string> operatorStack; // 运算符栈
        stack<Token> operandStack;   // 操作数栈

        operatorStack.push(";"); // 运算符栈底

        while (index < tokens.size())
        {

            if (tokens[index].type == "Constant" || tokens[index].type == "Identifier")
            { // 是操作数
                operandStack.push(tokens[index]);
                index++;
            }
            else if (tokens[index].type == "Operator" || tokens[index].type == "Delimiter")
            { // 是操作符
                string op = tokens[index].value;

                // 比较运算符优先级
                // case1: 如果栈顶运算符优先级高，则先计算，直到栈顶运算符优先级低
                while (icp[op] < isp[operatorStack.top()])
                {
                    string operatorStack_top = operatorStack.top();
                    operatorStack.pop();

                    Token operand2 = operandStack.top();
                    load(operand2, t1);
                    operandStack.pop();

                    Token operand1 = operandStack.top();
                    load(operand1, t2);
                    operandStack.pop();

                    if (operatorStack_top == "+")
                    {
                        MIPS::add(target, t0, t1, t2);
                    }
                    else if (operatorStack_top == "-")
                    {
                        MIPS::sub(target, t0, t1, t2);
                    }
                    else if (operatorStack_top == "*")
                    {
                        MIPS::mul(target, t0, t1, t2);
                    }
                    else if (operatorStack_top == "/")
                    {
                        MIPS::div(target, t0, t1, t2);
                    }
                    else if (operatorStack_top == "%")
                    {
                        MIPS::div(target, t0, t1, t2);
                        MIPS::mfhi(target, t0);
                    }
                    else if (operatorStack_top == "<")
                    {
                        MIPS::slt(target, t0, t1, t2);
                    }
                    else if (operatorStack_top == ">")
                    {
                        MIPS::slt(target, t0, t2, t1);
                    }
                    else if (operatorStack_top == "<=")
                    {
                        MIPS::slt(target, t0, t2, t1);
                        MIPS::xori(target, t0, t0, 1);
                    }
                    else if (operatorStack_top == ">=")
                    {
                        MIPS::slt(target, t0, t1, t2);
                        MIPS::xori(target, t0, t0, 1);
                    }
                    else if (operatorStack_top == "==")
                    {
                        MIPS::seq(target, t0, t1, t2);
                    }
                    else if (operatorStack_top == "!=")
                    {
                        MIPS::sne(target, t0, t1, t2);
                    }
                    else if (operatorStack_top == "|")
                    {
                        MIPS::or_(target, t0, t1, t2);
                    }
                    else if (operatorStack_top == "&")
                    {
                        MIPS::and_(target, t0, t1, t2);
                    }
                    else if (operatorStack_top == "^")
                    {
                        MIPS::xor_(target, t0, t1, t2);
                    }

                    operandStack.push(tempOperand);
                }

                // case2：如果优先级相等，则有两种情况，一种是遇到括号，一种是遇到分号
                if (icp[op] == isp[operatorStack.top()])
                {
                    if (op == ")")
                    { // 如果是右括号，必然是右括号遇到左括号，弹出左括号
                        operatorStack.pop();
                        index++;
                    }
                    else if (op == ";")
                    { // 如果是分号，则表达式结束
                        // 这里不需要index++，让指针停在分号上即可
                        break;
                    }
                }
                else
                {
                    // case3: 当前操作符优先级高于栈顶操作符，则压栈
                    operatorStack.push(op);
                    index++;
                }
            }
        }

        // 虽然在循环中已经将最终结果存入$t0，但是当表达式仅为一个操作数时，需要手动将结果存入$t0
        load(operandStack.top(), t0);

        return t0;
    }

    /**
     * description: 将Identifier或Constant加载到寄存器中
     * param: token - Identifier或Constant Token或Register Token
     */
    void load(Token token, string reg)
    {
        if (token.type == "Identifier")
        {
            MIPS::lw(target, reg, getIdentifierLoc(token.value), "fp");
        }
        else if (token.type == "Constant")
        {
            MIPS::li(target, reg, getConstantValue(token.value));
        }
        else if (token.type == "Register")
        { // 如果是寄存器，则直接move
            MIPS::move(target, reg, token.value);
        }
        else
        {
            cout << "Syntax Error, load() expect Identifier or Constant" << endl;
        }
    }

    /**
     * description: 将寄存器中的值存入Identifier变量（分配的指定栈空间中）
     */
    void assign(Token token, string reg)
    {
        if (token.type == "Identifier")
        {
            MIPS::sw(target, reg, getIdentifierLoc(token.value), "fp");
        }
        else
        {
            cout << "Syntax Error, assign() expect Identifier" << endl;
        }
    }

    /**
     * description: 删除main函数外框架，仅保留main函数内的代码（{}中的代码，不包括{}）
     * 局限性：不支持main函数内部再定义函数，即不支持嵌套函数（即全局仅能有一组{}）
     */
    void deleteMain()
    {
        // 直接在tokens中删除main函数外的代码
        vector<Token> newTokens;
        int i = 0;

        // 找到main函数，定位到{
        while (i < tokens.size())
        {
            if (tokens[i].type == "Keyword" && tokens[i].value == "int" && tokens[i + 1].type == "Keyword" && tokens[i + 1].value == "main" && tokens[i + 2].type == "Operator" && tokens[i + 2].value == "(")
            {
                i += 3;
                // 跳过main函数参数
                while (i < tokens.size() && !(tokens[i].type == "Delimiter" && tokens[i].value == "{"))
                {
                    i++;
                }
                break;
            }
            i++;
        }

        // 跳过{，找到main函数内的第一条语句
        i++;

        // 提取main函数内的代码
        while (i < tokens.size())
        {
            if (tokens[i].type == "Delimiter" && tokens[i].value == "}")
            {
                i++;
                break;
            }
            newTokens.push_back(tokens[i]);
            i++;
        }

        // 将main函数内的代码替换tokens
        tokens.swap(newTokens);
    }

    /**
     * description: 打印整数（syscall 1），并换行（syscall 11）
     * param: reg - 寄存器名
     */
    void Println(string reg)
    {
        MIPS::addi(target, "v0", "0", 1); // syscall code 1: print integer
        MIPS::move(target, "a0", reg);    // move reg to a0
        MIPS::syscall(target);
        MIPS::li(target, "v0", 11); // syscall code 11: print character
        MIPS::li(target, "a0", '\n');
        MIPS::syscall(target);
    }

    /**
     * description: 打印生成的MIPS汇编代码
     */
    void printTarget()
    {
        cout << target << endl;
    }

    /**
     * description: 打印tokens，用于调试，同时标记当前解析位置index
     */
    void printTokens()
    {
        cout << "Tokens:" << endl;
        for (int i = 0; i < tokens.size(); i++)
        {
            if (i == index)
                cout << ">>> ";
            cout << tokens[i].value << " ";
            if (tokens[i].type == "Delimiter")
                cout << endl;
        }
        cout << "Tokens end." << endl;
    }

    /**
     * description: 与outputTargetToFile()类似，将生成的MIPS代码插入模板字符串，返回最终的MIPS代码字符串
     */
    
    string getMIPSCode()
    {
        ifstream templateFile("../mips_template.s");

        if (!templateFile.is_open())
        {
            cout << "Error: template file not found" << endl;
            return "";
        }

        string templateCode((std::istreambuf_iterator<char>(templateFile)), std::istreambuf_iterator<char>());
        templateFile.close();
        templateCode.replace(templateCode.find("{{target}}"), 10, target); // 替换{{target}}为生成的MIPS汇编代码
        return templateCode;
    }
};


extern vector<Token> tokens;
extern Token thisToken;

void analyze_lexical(const char* filename);

FILE* set_input_file(const char *filename);
