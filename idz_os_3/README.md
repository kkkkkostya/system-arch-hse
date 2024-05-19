# ИДЗ №3. Татаров Константин Манолисович БПИ 228
**Варинат 12\
Условие задания:**

Задача о супермаркете. В супермаркете работают два кассира,
покупатели заходят в супермаркет, делают покупки и становятся в очередь к случайному кассиру. Пока очередь пустая, кассир «спит». Как только появляется покупатель, кассир «просыпается».
Покупатель ожидает в очереди, пока не подойдет к кассиру. Очереди имеют ограниченную длину N. Если она длиннее, то покупатель
не встает ни в одну из очередей и уходит. Если одна из очередей
заполнена, то покупатель встает в другую.
Создать многопроцессное приложение, моделирующее рабочий день супермаркета.
Каждый покупатель и кассиры задаются отдельными процессами.


```
Первая программа на 4-5 балла:
Описание сценария и как он отображается в процессы и их взаимодействие:

Исходные сущности и их поведение
Покупатель:

Покупатель приходит в супермаркет и выбирает кассира, к которому станет в очередь.
Если очередь кассира заполнена, покупатель проверяет очередь другого кассира.
Если обе очереди заполнены, покупатель уходит.
Покупатели создаются и распределяются клиентом покупателей.

Кассир:

Кассир "спит", когда очередь пуста.
Кассир "просыпается" и обрабатывает покупателя, когда он появляется в очереди.
Обработка занимает некоторое время.
Кассиры реализуются как клиенты.

Супермаркет:

Супермаркет управляет двумя кассирами и очередями.
Супермаркет распределяет покупателей по очередям кассиров.
Супермаркет реализуется как сервер.
Взаимодействие сущностей в программе

Сервер (супермаркет):

Запускается и ожидает подключения кассиров.
После подключения кассиров, сервер начинает принимать покупателей.
Сервер распределяет покупателей по очередям кассиров.
Если обе очереди заполнены, сервер сообщает клиенту покупателей, что он не может встать в очередь, и тот уходит.

Клиенты кассиры:

Подключаются к серверу.
Получают информацию о покупателях в очереди от сервера.
Обрабатывают покупателей по мере их появления в очереди.
Клиент покупателей:

Создаёт покупателей и отправляет их на сервер.
Получает от сервера информацию о том, был ли покупатель добавлен в очередь или ушёл.


Подробности реализации
Сервер:

Создаёт сокет и слушает на заданном порту.
Принимает подключения от кассиров и сохраняет их сокеты.
Создаёт отдельные потоки для обработки очередей каждого кассира.
Принимает подключения от клиентов покупателей.
Распределяет покупателей по очередям кассиров, или сообщает клиенту, что обе очереди заполнены.

Кассиры:

Подключаются к серверу и ожидают информацию о покупателях.
Запускают отдельный поток, который проверяет очередь и обрабатывает покупателей.
Отправляют серверу информацию о том, что они начали обработку покупателя.

Клиент покупателей:

Создаёт заданное количество покупателей.
Подключает каждого покупателя к серверу.
Получает от сервера информацию о том, был ли покупатель добавлен в очередь или ушёл.
```

[Результаты работы программы](https://github.com/kkkkkostya/system-arch-hse/blob/febe09d91582c0fee53bb76a936186f491acdb0e/idz_os_3/prog4/results4_1.png)

```
Программа на 6-7 баллов:

```