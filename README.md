# Hamming-Archivator

## Реализация

Консольное приложение, поддерживающее следующие аргументы командой строки:

**-c, --create - создание нового архива**

**-f, --file[ARCHNAME] - имя файла с архивом**

**-l, --list - вывести список файлов в архиве**

**-x, --extract - извлечь файлы из архива (если не указано, то все файлы)**

**-a, --append - удалить файл из архива**

**-A, --concatenate - слить воедино два архива**

Имена файлов передаются свободными аргументами

Аргументы для кодирования и декодирования так же передаются через командную строку

## Примеры запуска

*hamarc --create --file=ARCHIVE FILE1 FILE2 FILE3*

*hamarc -l -f ARCHIVE*

*hamarc --concatenate ARCHIVE1 ARHCIVE2 -f ARCHIVE3*
