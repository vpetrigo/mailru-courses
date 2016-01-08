# import sys
#
# inp = sys.stdin.readline().rstrip('\n')
# close_br = [')', ']', '}']
# open_br = ['(', '[', '{']
# stack = []
# right_ans = ""
# left_ans = ""
#
# def check_brackets(inp):
#     right_ans = ""
#     left_ans = ""
#
#     for br in inp:
#         if br in close_br:
#             if len(stack) == 0:
#                 if br == ')':
#                     left_ans = '(' + left_ans + br
#                 elif br == ']':
#                     left_ans = '[' + left_ans + br
#                 else:
#                     left_ans = '{' + left_ans + br
#             else:
#                 another_br = stack.pop()
#                 if (another_br == '(' and br == ')'
#                     or another_br == '[' and br == ']'
#                     or another_br == '{' and br == '}'):
#                         right_ans = another_br + right_ans + br
#                 else:
#                     print("IMPOSSIBLE")
#                     return []
#
#         elif br in open_br:
#             stack.append(br)
#
#     return [left_ans, right_ans]
#
# def check_brackets_in_stack(stack, right_ans):
#     while len(stack):
#         br = stack.pop()
#         if br == '(':
#             right_ans =  br + right_ans + ')'
#         elif br == '[':
#             right_ans = br + right_ans + ']'
#         else:
#             right_ans = br + right_ans + '}'
#     return right_ans
#
# ans = check_brackets(inp)
# if len(stack) != 0:
#     ans[1] = check_brackets_in_stack(stack, ans[1])
#
# # assert '{}[[([{[]}])]]' == ''.join(ans)
# print(''.join(ans))

import sys

CLOSE_BRACKETS = (')', ']', '}')
BRACKETS = dict([('(', ')'), (')', '('), ('[', ']'), (']', '['), ('{', '}'), ('}', '{')])

def compare_bracket_indexes(left_bracket, right_bracket):
    l_br_index = OPEN_BRACKETS.index(left_bracket)
    r_br_index = CLOSE_BRACKETS.index(right_bracket)

    return True if l_br_index == r_br_index else False

def check_brackets(inp):
    stack = []
    ans = ""

    for bracket in inp:
        if bracket in CLOSE_BRACKETS:
            if len(stack) == 0:
                ans = BRACKETS[bracket] + ans + bracket
            else:
                # take the last met bracket
                l_bracket = stack.pop()

                if not compare_bracket_indexes(l_bracket, bracket):
                    return []
                else:
                    ans += bracket
        else:
            # open bracket handling
            stack.append(bracket)
            ans += bracket

    # handle remaining brackets in stack
    while len(stack):
        bracket = stack.pop()
        close_bracket = get_close_bracket(bracket)
        ans = append_close_bracket(close_bracket, ans)

    return ans

def test():
    assert "{}[[([{[]}])]]" == check_brackets("}[[([{[]}"), check_brackets("}[[([{[]}")
    assert [] == check_brackets("{][[[[{}[]")
    assert [] == check_brackets("[)"), check_brackets("[)")
    assert "{[]()}" == check_brackets("]()}"), check_brackets("]()}")
    assert "()[]" == check_brackets("()[]"), check_brackets("()[]")
    assert "[()[]]" == check_brackets("[()[]"), check_brackets("[()[]")
    assert "[[]]" == check_brackets("[["), check_brackets("[[")

def main():
    inp = sys.stdin.readline().rstrip('\n')
    ans = check_brackets(inp)

    if ans:
        print(ans)
    else:
        print("IMPOSSIBLE")

if __name__ == "__main__":
    main()
