import subprocess
from os import name


def run_asm_file(asm_file_path, input_data):
    extra_spaces = 66  # Это значение может измениться в зависимости от формата вывода

    try:
        count=1
        for input in input_data: 
            print(f'Тест номер {count}')
            print()
            result = subprocess.run( #Запуск программы, передавая ей на вход тестовые данные(параметр input).
                #Третьим параметом должен стоять полный путь к rars на вашем компьютере
                ["java", "-jar", "C:/Users/kostya_/Downloads/Telegram Desktop/rars1_6.jar", asm_file_path],
                input=input, text=True, capture_output=True)
            print("Входные данные:\n" + input)
            print("Результат работы программы:")
            print(result.stdout[extra_spaces:])  # Вывод результата (может потребовать настройки)
            print()
            count+=1

    except subprocess.CalledProcessError as e:
        print("Произошла ошибка:", e)


def main():
    asm_file_path = "C:/Users/kostya_/OneDrive/Рабочий стол/ABC/ИДЗ/main.asm" #Путь к main файлу

    test_data = [
        "0",
        "11",
        "-5",
        "1\n-10",
        "1\n10",
        "1\n5",
        "1\n-5",
        "6\n-4\n-3\n-2\n-1\n3\n4",
        "8\n6\n7\n12\n144\n26\n11\n19\n30",
        "4\n-12\n-30\n4\n15",
        "5\n-1234\n12\n0\n5\n11"
    ]

    run_asm_file(asm_file_path, test_data)


if __name__ == "__main__":
    main()
