# 恺撒密码 B
str = input("")
for i in str:
    if "a" <= i <= "z":
        print(chr(ord("a") + (ord(i) - ord("a") + 3) % 26), end="")  # 注意算法
    elif "A" <= i <= "Z":
        print(chr(ord("A") + (ord(i) - ord("A") + 3) % 26), end="")
    else:
        print(i, end="")
