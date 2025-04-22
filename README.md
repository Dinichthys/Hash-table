# Хеш-таблица

## Аннотация

### Цель работы

Реализовать структуру данных - хеш-таблицу с закрытой адресацией и разрешением конфликтов методом цепочек. После реализации требуется оптимизировать написанную программу тремя разными способами.

### Оборудование

<table>
    <thead>
        <tr>
            <th align="center" colspan = 2>Характеристики экспериментальной установки</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td align="center">ОС</td>
            <td align="center">Ubuntu 24.04.2 LTS <p>
            Linux Kernel 6.11.0-21-generic</td>
        </tr>
        <tr>
            <td align="center">Процессор</td>
            <td align="center">AMD Ryzen 7 7730U with Radeon Graphics
            <p>
            16 ядер
            </td>
        </tr>
        <tr>
            <td align="center">Частота процессора на момент измерений</td>
            <td align="center">2 ГГц
            </td>
        </tr>
        <tr>
            <td align="center">Версия G++</td>
            <td align="center">13.3.0
            </td>
        </tr>
        <tr>
            <td align="center">Версия GLIBC</td>
            <td align="center">2.39
            </td>
        </tr>
    </tbody>
</table>

Фиксирование частоты процессора происходило с помощью команд:
``` bash
sudo cpupower frequency-set -u 2010Mhz
sudo cpupower frequency-set -d 1990Mhz
```
После завершения снятия измерений фиксация сбрасывалась командой:
``` bash
sudo cpupower frequency-set -g ondemand
```

## Теоретическая справка

### Структура Хеш-таблицы

Хеш-таблица представляет из себя массив бакетов (списков), в которой при добавлении элемента индекс бакета, в который нужно его добавить, определялся по формуле

``Индекс бакета := Хеш элемента % Количество бакетов``

В бакетах же, которыми являются списки, хранятся слова и количество их добавлений.

### Оптимизации

Для анализа программы создавалась хеш-таблица на базе текста всех произведений из серии книг о Гарри Поттере.

## Ход работы

### Профилирование

Для определения горячих мест в программе был использован профилировщик `perf`, так как данный профилировщик поддерживается на аппаратном и системном уровнях.

Профиль создавался с помощью следующей команды:

``` bash
sudo perf record --call-graph dwarf ./build/hash-table
```

А просмотр профиля происходил с помощью `perf report`, который вызывался следующей командой:

``` bash
sudo perf report --no-children
```

Для построения графов использовался `hotspot`:

``` bash
sudo hotspot
```

Все вызовы происходили в режиме суперпользователя, чтобы в профиле имена системных функций были видны, а не замещались их адресами.

### Стартовая версия

#### Описание

Стартовая версия включала в себя составление таблицы из слов в виде строк (`char*`).

#### Результат профилирования

<table>
    <thead>
        <tr>
            <th align="center">Профиль</th>
            <th align="center">Граф</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td align="center">
            <img src="data/Perf1_highlighted.png" alt="Фото">
            </td>
            <td align="center">
            <img src="data/FlameGraph1.png" alt="Фото">
            </td>
        </tr>
    </tbody>
</table>

### Первая оптимизация

#### Описание

В профиле первой программы можно увидеть, что самая горячая функция - `strcmp`.

Данный результат приводит нас к выводу о том, что нужно оптимизировать работу со строками. Сравнение строк можно сделать с помощью **AVX** инструкций. Для этого их нужно будет загружать в широкие регистры, но длина регистров ограничена, поэтому для данной оптимизации ограничим длину допустимых слов. Будем считать, что количество слов длины большей **32** пренебрежимо мало. Тогда заменим работу со строками на **SIMD** интринсики. Также, будем считать, что аргументы функции выравнены по **32**.

<details>
<summary> Реализация функции на языке C </summary>

``` C
int strcmp_256 (const char* const first_str, const char* const second_str)
{
    ASSERT (first_str != NULL, "First string is null pointer in function strcmp\n");
    ASSERT (second_str != NULL, "Second string is null pointer in function strcmp\n");

    __m256i first_m256  = _mm256_load_si256 ((__m256i*) first_str);
    __m256i second_m256 = _mm256_load_si256 ((__m256i*) second_str);

    if (_mm256_movemask_epi8 (_mm256_cmpeq_epi32 (first_m256, second_m256)) == 0xFF'FF'FF'FF)
    {
        return 0;
    }

    return kNEqual;
}
```
</details>

#### Результат профилирования

<table>
    <thead>
        <tr>
            <th align="center">Профиль</th>
            <th align="center">Граф</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td align="center">
            <img src="data/PerfStrCmp_highlighted.png" alt="Фото">
            </td>
            <td align="center">
            <img src="data/FlameGraphStrCmp.png" alt="Фото">
            </td>
        </tr>
    </tbody>
</table>

#### Результат оптимизации

<table>
    <thead>
        <tr>
            <th align="center">Версия, относительно которой измеряется прирост производительности</th>
            <th align="center">Во сколько раз ускорилась программа</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td align="center">
            Стартовая версия
            </td>
            <td align="center">
            1.306 ± 0.001
            </td>
        </tr>
    </tbody>
</table>

### Вторая оптимизация

#### Описание

Полученный профиль показывает, что `strcmp` оптимизировалась, но всё равно осталась горячей. Однако, можно заметить, что она вызывается в основном из выделенной в прямоугольник в профиле функции. Эта функция тоже является горячей, поэтому попробуем оптимизировать её. Данная функция содержит в себе лишь один цикл с поиском элемента и больше ничего, что означает, что оптимизация её возможна, например, в случае переписывания её на язык **ассемблера**. Такое могло случиться, если компилятор не смог учесть все задержки по времени на работу с памятью и не достаточно хорошо распределил инструкции.

Перевод данной функции в ассемблер можно посмотреть по ссылке на [GodBolt](https://godbolt.org/#z:OYLghAFBqd5QCxAYwPYBMCmBRdBLAF1QCcAaPECAMzwBtMA7AQwFtMQByARg9KtQYEAysib0QXACx8BBAKoBnTAAUAHpwAMvAFYTStJg1DIApACYAQuYukl9ZATwDKjdAGFUtAK4sGIAMwArKSuADJ4DJgAcj4ARpjEIADspAAOqAqETgwe3r4BwemZjgLhkTEs8YkpdpgO2UIETMQEuT5%2BQbaY9iUMjc0EZdFxCcm2TS1t%2BZ0KE4MRw5WjSQCUtqhexMjsHOb%2BEcjeWADUJv5us%2Bi0eLEAdAhn2CYaAIJ7B0eYp%2BdsLCQAnvdHs83mZ9gxDl4TmcLgRiBFgED/E9Xu8IZ9vm48CwWBE4REkSi3q9ZkxHMhjmgGLNjpkAF6YAD6BGOAGsALJMVQAdRI6FCjG%2BABFjv4zGcrCSmuTaXhgJF0MdaAJgEqVWzlKg8AoBAA1MTC44AWi4EpBIII/1SmCwVFpcK8DmOCCYCgQjO6mBY5qSkpexwDxzEcuYCmOEEZjJYZkCADYVpSXcR7fCjKdAhYOVzecR%2BYwTIEhWbXoHZQzmZSNoIEsW3kkRS63R76CxmbWLVabZg7bNiI6WXyEozrrMfX7S/SmSzIqoCO2S4HJxXUsRMAA3JxeBTt%2BvHQfEYfaght/x%2BjvW20p/tKo9jkGltda9AAKmO6DJTHn/sD%2B8Ps2Zr77l%2BE54OWLJoF41bEMBi6gVOZaYDBAZLiyVCrohp7mguyFwRWnqtpOO4iiOx5zphqIUS8mTyjaappsqabhLMABiEToNgLZCAAkuyIoQFSNIkQBlbUiyJGkCJNLIEmr4CSy%2BGMAQKx3thibNMcCmCIyvbppmnI8nyAoMAWIpnKZZjihZtYTnCyCpP84aacevYSU5ynkcS34BpG0ZxngGktopjLcbxhpRr5sbDqgTDoNpeAxrG4b8QINI%2BQleDPgmTnaXC7lnqpKE3v%2B%2BGMmxmCqIaJFGo8QEZhoJm3DOZH5V5zquu6JUEK%2BjYdS2jLNMmZnhj1zZegBKwQFVjzvk0eVYa1aV%2BZSLCpKNLBhTiCXaZgBAMsQqBxQl4ZzapADuCB0F8k1HmtpUMFgFVgGAQ0aMpqkmL696loGyArWtG0RVFMWHXGSVycci2xhlCYjSVA0FhYQklWVqgNb2CIna1pZ4HaEabXGUaoGuXquqyHqpHgAAc4bhVtv3WgAjuTeBio5gVaSFQoSfTa0rAmZkvaozHMWAHDC6L4ti8xb1Y4GH3jt9iurgQmwMOG1EKnRqoMcACZI31KPWYrpz1l9iv62NKOVUe1XIrViM3cj93lQ1TVGwGH1FpRpbK6rGpajqDD6rQRHzRway0JwgS8H4HBaKQqCcG41jWLSGxbF8ew8KQBCaOHaysiAkixrcACckhJIEgQaJXST%2BJIXCxsEkccJIMd5wnnC8AoIAaDnedrHAsBIGgK2XWQFDJWP9CJMAXD%2BP4fB0AQCQ9xAsQd7EETNP8nDZ1vzDEP8ADysTaHUufcLwo9sIIx8MLQu9x7wWCxF4wBuGItA91fpBYCwhhgDiGfn/PAq56jEx/vHcqdQvArz3rwPE3QO7XFiMQHeHgsAd3xCwBBpBibEFiBkTAQoSZGGuEYAefADDAAULqPAmBTrH2tLHbO/BBAiDEOwKQMhBCKBUOoEBuguD6EASgFOlh9A3B7pANYqBUi9B/kaY%2BZhjQALXKoMwvAiYJHhFgGREA1i1HqM4CArgph%2BBEWEBYFQqh6CKFkAQFj7EZEcQwIYtjRgiOMb0fokxPDtD0D4hocwPEjESN4uYzjIkDDCUsCJRj07bD0KdQwBBmEEC4gwfgCCI5R3biAxOHBjiqEprGI0sZJDHGAMgCk89bj%2BHDLgQgJBThgi4CsXgl8tB81IIXQI/hbgaECGYSQkhS6jLME3SmQRm6cDbqQWO8cind17v3Z%2BvSW5aMWR3FZ6yelrAIZkZwkggA%3D%3D).

<details>
<summary> В случае, если ссылка не работает, можно посмотреть на скриншот с GodBolt </summary>
<img src="data/GodBoltFunc.png" alt="Фото">
</details>

Можно заметить, что загрузка слова из памяти в широкий регистр происходит непосредственно в теле цикла перед сравнением, что замедляет программу, так как происходит зависимость по данным.

При переводе на язык ассемблера данной функции нужно было учесть, что загрузка из памяти **256** бит информации - долгий процесс, поэтому простой перевод данной функции нам бы не сильно помог. Для лучшего результата сделаем так:

1. Заранее поставим на загрузку слово в регистр **ymm2** (ещё до цикла)
2. Как только понадобиться это слово, переложим его из **ymm2** в **ymm1**
3. Сразу после этого (до сравнения **ymm1** с искомым словом) поставим следующее в списке слово на загрузку в **ymm2**.

Таким образом, загрузка слова в широкий регистр **ymm2** всегда будет происходить параллельно с другими операциями, а в случае необходимости сравнения закаченного слова с искомым, значение из одного широкого регистра **ymm2** будет перекладываться в другой широкий регистр **ymm1**, что быстрее, чем доступ в память.

<details>
<summary> Реализация функции на языке <b>Assembly</b> (<i>NASM x86-64</i>) </summary>

``` asm
ASMListFindElemSIMD:
    push rbp
    mov rbp, rsp

    mov rax, rsp
    and rax, kMaskAlign
    add rsp, rax
    sub rsp, kAlignment

    push kEndSym
    push kEndSym
    push kEndSym
    push kEndSym                            ; Place in stack for buffer

    mov rcx, kWordLen

.WriteWordToStack_Comparison:
    cmp byte [rsi], kEndSym
    je .WriteWordToStack_Stop

    test rcx, rcx
    je .WriteWordToStack_Error

.WriteWordToStack_Body:
    mov al, byte [rsi]
    mov byte [rsp], al
    inc rsp
    inc rsi
    dec rcx
    jmp .WriteWordToStack_Comparison
.WriteWordToStack_Stop:                    ; Слово было записано в стек

    mov rsi, rcx
    mov rcx, kWordLen
    sub rcx, rsi
    sub rsp, rcx                           ; Установил RSP на начало слова

    vmovaps ymm0, yword [rsp]              ; YMM0 = искомое слово
    mov rcx, qword [rdi + 2*kSizePointer]  ; RCX = количество элементов в списке
    mov rdi, qword [rdi]                   ; RDI = указатель на массив с данными
    mov rdx, 1                             ; RDX = индекс взятого элемента

    mov r9, rdi
    add r9, 2 * kWordLen                   ; R9 = указатель на выбранное из списка слово

;-----------НАЧАЛО---ОПТИМИЗИРОВАННОГО---БЛОКА---------

    inc rcx
    vmovaps ymm2, yword [r9]

;------------------------ЦИКЛ---------------------------

.FindElemInList_Comparison:

    cmp rdx, rcx                          ; Счётчик RDX перебрал все элементы
    jae .WordNotFound

    vmovaps ymm1, ymm2
    add r9, 2 * kWordLen                  ; R9 = указатель на следующее слово в списке

    vmovaps ymm2, yword [r9]              ; Заранее поставили на загрузку следующего слова в YMM2
    vpxor ymm3, ymm1, ymm0                ; Сравнение

    inc rdx

    vptest ymm3, ymm3                     ; Проверка, что YMM3 == 0

    jne .FindElemInList_Comparison

;-------------------------------------------------------

;---------------ВОЗВРАЩАЕМЫЕ---ЗНАЧЕНИЯ-----------------

.FindElemInList_Stop:
    mov rsp, rbp
    pop rbp
    mov rax, rdx
    mov rdi, rdx
    ret

.WordNotFound:
    mov rsp, rbp
    pop rbp
    mov rax, kPoison
    mov rdi, rax
    ret

.WriteWordToStack_Error:
    mov rsp, rbp
    pop rbp
    mov rax, kTooLongWordError
    mov rdi, rax
    ret
```

</details>

#### Результат профилирования

<table>
    <thead>
        <tr>
            <th align="center">Профиль</th>
            <th align="center">Граф</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td align="center">
            <img src="data/Perf3_highlighted.png" alt="Фото">
            </td>
            <td align="center">
            <img src="data/FlameGraph3.png" alt="Фото">
            </td>
        </tr>
    </tbody>
</table>

#### Результат оптимизации

<table>
    <thead>
        <tr>
            <th align="center">Версия, относительно которой измеряется прирост производительности</th>
            <th align="center">Во сколько раз ускорилась программа</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td align="center">
            Стартовая версия
            </td>
            <td align="center">
            2.95 ± 0.04
            </td>
        </tr>
        <tr>
            <td align="center">
            Предыдущая версия
            </td>
            <td align="center">
            2.26 ± 0.04
            </td>
        </tr>
    </tbody>
</table>

### Третья оптимизация

#### Описание

Полученный после предыдущей оптимизации профиль показывает, что функция не перестала быть горячей, но дальнейшая её оптимизация затруднительна, так как функция уже написана в ускоренной её версии на языке **ассемблера**. В таком случае, оптимизировать нужно следующую горячую функцию - функцию поиска элемента в хеш-таблице.

<details>
<summary> Как выглядит функция, которую нужно оптимизировать? </summary>

``` C
int64_t ASMHashTableFindElemSIMD (hash_table_t hash_table, const char* const element)
{
    const size_t bucket_index = Hashing (element, strlen (element)) % kNumBucket;

    const int64_t val_index =
    ASMListFindElemSIMD (&hash_table [bucket_index], element);

    if (val_index == kPoisonVal)
    {
        return kPoisonVal;
    }

    return (int64_t) ((hash_elem_t*)(hash_table [bucket_index].data))[val_index].counter;
}
```

</details>

Перевод её на язык ассемблера можно посмотреть на сайте [GodBolt](https://godbolt.org/#z:OYLghAFBqd5QCxAYwPYBMCmBRdBLAF1QCcAaPECAMzwBtMA7AQwFtMQByARg9KtQYEAysib0QXACx8BBAKoBnTAAUAHpwAMvAFYTStJg1DIApACYAQuYukl9ZATwDKjdAGFUtAK4sGISQDMpK4AMngMmAByPgBGmMQgAOwAnKQADqgKhE4MHt6%2B/kEZWY4CYRHRLHEJKbaY9qUMQgRMxAR5Pn6BdQ05za0E5VGx8UmpCi1tHQXdEwNDldVjAJS2qF7EyOwc5gHhyN5YANQmAW4T6LR4MQB0CKfYJhoAgrv7h5gnZ2wsJACedweT1eZj2DAOXmOp3OBGI4WAgICjxeb3BHy%2BbjwLBY4Vh4URyNeLzmjmQRzQDAmRyyAC9MAB9AhHADWlQsXmQzMwTNOABEjmYwBwNCLTlZiS1SeSBFTaQymcyALJMVQAdRI6BCjC%2B/ICZjFwOBJLwZKywAi6COtAEwCtNpZylQeAUAgAamIdUcALRcA0ol4EP5pTBYKjU2EcpkIJgKBD0%2BqYFiGxLi55HdNHMR4c0xo4Qen0lhmACsADZluTo8Rw3CjCdixYlSr1cRNYwTMXeX60xm5YzpV5BPFuyZEvzo7H4/QWIyRwGgyHMGGJsRI0cNfF6VcJsnUxnqXg6f2IqoCHOe%2Bm%2B0y0sRMAA3JxeBQjsfr1ub7cEWcBVPAwPB0MazXT9d2Bfc7yddAACojnQJgWnPfcN2ILdnS/AgYOQxDe0PeUByHYhsMvXD%2BzlIiDyPJkqFvTByKvI4ExnMif2TflP2/X95wApcrTQ/sYg5Ll0PPf9FzDATOW5fsJzjFoYnofsO0bNlBO5DsuxY/1nkwU94gYE4zDMNxzDMI5cVLSR%2B2eIRFTCCYADFwnQbBpyEABJRV%2BQgCkqXYjDpUpJlP1IAKqWQKsYJ8plGMYAhlnPeiAAkYwQeE8yiytWkimUmTvMQQoy%2BiGB8eLNKJZ5zMsplrMVZLYwAFSYeTMEchhnNcjyvJkxkmoUqMUp65qCpyzLiGywKGOnWL4pRFMwIzQqSKZCShPpJydM9OrUrrCAYsEEKV3ofTdqmwRlgrcxixZFTJLPMr5vTDLKv7PLaDWtqNr5B6jhquyCFa9rE3czy83MUturk%2Bh6wsFapPW1R1JCva4vIvAwwgV73qwVQdT5B0nRdBh3VoGaLxOOaXn3fdbwIDZ9OZR1nTdMRu33Ucuy06nuTpvNnrivMIG6xjGSg5ZBYGyHPiU2Gv3h9SbjglpzqUzG5c7G40EHAhh3usdDRePmjjchgrgiGqtsa5qAZcoHOrzCHerwh2htCplwqy13JsTabQMpjNDZp%2BlXs9BmCeZ2hWYWkb6JlrHPoCfktrSk7vf2msjrzZHzoMq7WR8dlbvImMWAgb62cM347wARxzrgQsuy7WzwDs3AYFuz0Mqmu6OAB6HuTl5DQTGSeyTEHkwLGLMffQADiH8fJ7HoeLEkJeJ/n2fEjXiwNAH5eN5MOft4CafD5PhfjIXqeF67XlZ/1BfHgX/U/a7kzK5ry6zHrktG6yFu27FjcB3Uy3d9x9z3hPUyp8Z6rzvmfbet9l6vHgTPUs28Z4wOwJAiwo9H4YJgb6BeKD94D1nkgieVgr7b0vsva%2By9b7323rvbe0DUFEOXng5emD2HbxIRPKer8qYmVELQWgP0bJ/Wth1TyAD24mX1mTcuZgFBeBiDXAAJHqH%2BU8SzEGSHIoBICwHgP7mvZIW8F4HyPlYxB296ECJwVwieT9Z5EM3gQ1BD9972JoTgyxDDmEwICQIuxC8eF0KOAAJWSJ6GJ3ojh6jLhmERLA0iaJ9DoxuKpDHAJMiY9MEC17PHcYfeePjn5hO4X4whZDD7OJXtvJ%2By9vETx4Uw1BVDl5uH8VU9ewTfE308XQmhyT0wmW0J8G4QhmR4DSDVXJIDFFgPfqgauoNf56OSKsHOQ8gGALyZ3ApRTx4WKCQvBp7TD4RNCRfHBbDZ7lNucvSxs9z7LyobPS%2BHSKn3zqTPUpM9XnD0BQ/Dx4S%2BFjIMmYbQaSjg3GwG1BZ%2Bz5GGShSZaZsz5k2RAIshRQjlEfyOBozJuy8VHIKYUsxpyQk723pcmBNzF53NYf8p5zKXkwPeZQmB3zD6tLpV4/5DS4HL2aVAzxPyXH2N6UM2xqDaUOIsBQiwTLBFKJSYZBFSKcXkpfhq9MIBoV8kIp3CAgdXqkzAUakyprTKlzMODCWjtoaxzVryHZtr8kpzYGdKFNrDLEByYZH%2BZgm4mVDcQf%2BIboWtgRjG21yB43f1jQYhNgbMFoqEaVTiZMaY8wtSzXWvIOCrFoJwYsvA/DCl4KgTgxlLDWGpOsTYUtQQ8FIAQTQpbVjMn8IkG4yRSzFg0AEAIM9x2GUSNO/QnBJBVu7aQOtHBeAKBABoTt3bVhwFgEgNAaS6DxHIJQfdaRD0JGAFwMdfA6DayjZQGIi6YjhFaH8TgHbn3MGIH8AA8jESZDh328H3b6ggP6TZvpraQLAAlgBuDELQNd3BeBYBYIYYA4goP4FvA4PAd5MBIa0MEVQmBkBeG1kB8gQ5y1QauDEIN36PBYEXXiFglH8PEBiJkTAvJEzodNqAGtqwqAGGAAoV0eBMAAHcf3BmrR2/gggRBiHYFIGQghFAqHUFB3QdcDBGBQNYaw%2BhrhrsgKsVAaRGhIa9D%2B0yXo0N3lUGYWtHG4RYDMxAVYdhSONBcG1aYfg66hHCMMKoow67FGyAIQLegouNAWCMBIdcfO4YEP0KYnhOh6FS40DLgxQuLAi7YSY7QssFBS6VxL4XkveZbVsPQUnDBgas8bfgQGy0VoXVB5dRxVBoK9BZI4wBkBkivTcAIeZcCEBIAZAIXBli8C7UJ3tIBixmBuKOyQQ7SxcF29Owy0gaPztINWojy7V3rs3St2dHAXOncXRd67WhzqkA41kZwkggA%3D%3D%3D).

<details>
<summary> В случае, если ссылка не работает, можно посмотреть на скриншот с GodBolt </summary>
<img src="data/GodBoltASMHashTableFindElemSIMD.png" alt="Фото">
</details>

В данной функции происходит лишь расчёт хеша элемента, а затем поиск этого элемента в выбранном списке. Однако, можно заметить, что возвращаемое значение этой функции содержит несколько доступов в память, а значит, эту часть функции и нужно улучшить.

Частично функцию можно оптимизировать с помощью **inline assembly**. Им и воспользуемся.

Заметим, что в данном выражении происходит доступ к счётчику количества добавления слова в таблицу. Вспомним, что перед этим мы вызывали функцию поиска слова в списке, написанную на языке ассемблера, то есть мы знаем, как в ней между регистрами будут распределены данные. В таком случае запишем на выходе из функции поиска элемента в списке её результат - **RAX** - в регистр **RDI**, а в начале ассемблерной вставки вернём его в **RAX**. Сравним значение **RAX** с **-1** (ядовитое значение), и в случае равенства выйдем из функции.

В противном случае вспомним, что в **R9** лежал указатель на элемент, следующий за найденным, то есть счётчик в найденном элементе должен лежать по адресу **R9** - 32, так как структура была увеличена до 32 байт с целью выравнивания данных по 32 с целью работы с **AVX** инструкциями. Таким образом, положим эту величину в **ret_val** и выйдем из функции.

<details>
<summary> Реализация функции с помощью <b>asm ()</b> </summary>

``` C
int64_t InlineASMHashTableFindElemSIMD (hash_table_t hash_table, const char* const element)
{
    int64_t ret_val = kPoisonVal;
    const size_t bucket_index = Hashing (element, strlen (element)) % kNumBucket;

    asm(
        "movq %1, %%rdi\n\t"            // Передача аргументов
        "movq %2, %%rsi\n\t"            // в функцию поиска элемента в списке
        "call ASMListFindElemSIMD\n\t"

        "subq $32, %%r9\n\t"            // Заранее посчитаем значение R9 = R9 - 32
        "cmpq $-1, %%rax\n\t"           // Сравним с ядовитым значением
        "je .SkipASM\n\t"

        "movq (%%r9), %0\n\t"           // Запишем в результат счётчик
        "jmp .EndASM\n\t"
        ".SkipASM:\n\t"
        "movq $-1, %0\n\t"              // Запишем в результат ядовитое значение
        ".EndASM:\n\t"
        : "=r" (ret_val)
        : "r" (&hash_table [bucket_index]), "r" (element)
        : "rax", "rdi", "rsi", "rdx", "rcx", "r9", "r8"
    );

    return ret_val;
}
```

</details>

Новый перевод функции в язык ассемблера и сравнение двух переводов можно посмотреть на сайте [GodBolt](https://godbolt.org/#z:OYLghAFBqd5QCxAYwPYBMCmBRdBLAF1QCcAaPECAMzwBtMA7AQwFtMQByARg9KtQYEAysib0QXACx8BBAKoBnTAAUAHpwAMvAFYTStJg1DIApACYAQuYukl9ZATwDKjdAGFUtAK4sGISQDMpK4AMngMmAByPgBGmMQgAOwAnKQADqgKhE4MHt6%2B/kEZWY4CYRHRLHEJKbaY9qUMQgRMxAR5Pn6BdQ05za0E5VGx8UmpCi1tHQXdEwNDldVjAJS2qF7EyOwc5gHhyN5YANQmAW4T6LR4MQB0CKfYJhoAgrv7h5gnZ2wsJACedweT1eZj2DAOXmOp3OBGI4WAgICjxeb3BHy%2BbjwLBY4Vh4URyNeLzmjmQRzQDAmRyyAC9MAB9AhHADWlQsXmQzMwTNOABEjmYwBwNCLTlZiS1SeSBFTaQymcyALJMVQAdRI6BCjC%2B/ICZjFwOBJLwZKywAi6COtAEwCtNpZylQeAUAgAamIdUcALRcA0ol4EP5pTBYKjU2EcpkIJgKBD0%2BqYFiGxLi55HdNHMR4c0xo4Qen0lhmACsADZluTo8Rw3CjCdixYlSr1cRNYwTMXeX60xm5YzpV5BPFuyZEvzo7H4/QWIyRwGgyHMGGJsRI0cNfF6VcJsnUxnqXg6f2IqoCHOe%2Bm%2B0y0sRMAA3JxeBQjsfr1ub7cEWcBVPAwPB0MazXT9d2Bfc7yddAACojnQJgWnPfcN2ILdnS/AgYOQxDe0PeUByHYhsMvXD%2BzlIiDyPJkqFvTByKvI4ExnMif2TflP2/X95wApcrTQ/sYg5Ll0PPf9FzDATOW5fsJzjFoYnofsO0bNlBO5DsuxY/1nkwU94gYE4zDMNxzDMI5cVLSR%2B2eIRFTCCYADFwnQbBpyEABJRV%2BQgCkqXYjDpUpJlP1IAKqWQKsYJ8plGMYAhlnPeiAAkYwQeE8yiytWkimUmTvMQQoy%2BiGB8eLNKJZ5zMsplrMVZLYwAFSYeTMEchhnNcjyvJkxkmoUqMUp65qCpyzLiGywKGOnWL4pRFMwIzQqSKZCShPpJydM9OrUrrCAYsEEKV3ofTdqmwRlgrcxixZFTJLPMr5vTDLKv7PLaDWtqNr5B6jhquyCFa9rE3czy83MUturk%2Bh6wsFapPW1R1JCva4vIvAwwgV73qwVQdT5B0nRdBh3VoGaLxOOaXn3fdbwIDZ9OZR1nTdMRu33Ucuy06nuTpvNnrivMIG6xjGSg5ZBYGyHPiU2Gv3h9SbjglpzqUzG5c7G40EHAhh3usdDRePmjjchgrgiGqtsa5qAZcoHOrzCHerwh2htCplwqy13JsTabQMpjNDZp%2BlXs9BmCeZ2hWYWkb6JlrHPoCfktrSk7vf2msjrzZHzoMq7WR8dlbvImMWAgb62cM347wARxzrgQsuy7WzwDs3AYFuz0Mqmu6OAB6HuTl5DQTGSeyTEHkwLGLMffQADiH8fJ7HoeLEkJeJ/n2fEjXiwNAH5eN5MOft4CafD5PhfjIXqeF67XlZ/1BfHgX/U/a7kzK5ry6zHrktG6yFu27FjcB3Uy3d9x9z3hPUyp8Z6rzvmfbet9l6vHgTPUs28Z4wOwJAiwo9H4YJgb6BeKD94D1nkgieVgr7b0vsva%2By9b7323rvbe0DUFEOXng5emD2HbxIRPKer8qYmVELQWgP0bJ/Wth1TyAD24mX1mTcuZgFBeBiDXAAJHqH%2BU8SzEGSHIoBICwHgP7mvZIW8F4HyPlYxB296ECJwVwieT9Z5EM3gQ1BD9972JoTgyxDDmEwICQIuxC8eF0KOAAJWSJ6GJ3ojh6jLhmERLA0iaJ9DoxuKpDHAJMiY9MEC17PHcYfeePjn5hO4X4whZDD7OJXtvJ%2By9vETx4Uw1BVDl5uH8VU9ewTfE308XQmhyT0wmW0J8G4QhmR4DSDVXJIDFFgPfqgauoNf56OSKsHOQ8gGALyZ3ApRTx4WKCQvBp7TD4RNCRfHBbDZ7lNucvSxs9z7LyobPS%2BHSKn3zqTPUpM9XnD0BQ/Dx4S%2BFjIMmYbQaSjg3GwG1BZ%2Bz5GGShSZaZsz5k2RAIshRQjlEfyOBozJuy8VHIKYUsxpyQk723pcmBNzF53NYf8p5zKXkwPeZQmB3zD6tLpV4/5DS4HL2aVAzxPyXH2N6UM2xqDaUOIsBQiwTLBFKJSYZBFSKcXkpfhq9MIBoV8kIp3CAgdXqkzAUakyprTKlzMODCWjtoaxzVryHZtr8kpzYGdKFNrDLEByYZH%2BZgm4mVDcQf%2BIboWtgRjG21yB43f1jQYhNgbMFoqEaVTiZMaY8wtSzXWvIOCrFoJwYsvA/DCl4KgTgxlLDWGpOsTYUtQQ8FIAQTQpbVjMn8IkG4yRSzFg0AEAIM9x2GUSNO/QnBJBVu7aQOtHBeAKBABoTt3bVhwFgEgNAaS6DxHIJQfdaRD0JGAFwMdfA6DayjZQGIi6YjhFaH8TgHbn3MGIH8AA8jESZDh328H3b6ggP6TZvpraQLAAlgBuDELQNd3BeBYBYIYYA4goP4FvA4PAd5MBIa0MEVQmBkBeG1kB8gQ5y1QauDEIN36PBYEXXiFglH8PEBiJkTAvJEzodNqAGtqwqAGGAAoV0eBMAAHcf3BmrR2/gggRBiHYFIGQghFAqHUFB3QdcDBGBQNYaw%2BhrhrsgKsVAaRGhIa9D%2B0yXo0N3lUGYWtHG4RYDMxAVYdhSONBcG1aYfg66hHCMMKoow67FGyAIQLegouNAWCMBIdcfO4YEP0KYnhOh6FS40DLgxQuLAi7YSY7QssFBS6VxL4XkveZbVsPQUnDBgas8bfgQGy0VoXVB5dRxVBoK9BZI4wBkBkivTcAIeZcCEBIAZAIXBli8C7UJ3tIBixmBuKOyQQ7SxcF29Owy0gaPztINWojy7V3rs3St2dHAXOncXRd67WhzqkA41kZwkggA%3D%3D%3D).

<details>
<summary> В случае, если ссылка не работает, можно посмотреть на скриншот с GodBolt </summary>
<img src="data/GodBoltInlineASMHashTableFindElemSIMD.png" alt="Фото">
</details>

Теперь в функции есть всего один доступ в память, из-за чего она работает немного быстрее.

#### Результат профилирования

<table>
    <thead>
        <tr>
            <th align="center">Профиль</th>
            <th align="center">Граф</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td align="center">
            <img src="data/Perf4.png" alt="Фото">
            </td>
            <td align="center">
            <img src="data/FlameGraph4.png" alt="Фото">
            </td>
        </tr>
    </tbody>
</table>

#### Результат оптимизации

<table>
    <thead>
        <tr>
            <th align="center">Версия, относительно которой измеряется прирост производительности</th>
            <th align="center">Во сколько раз ускорилась программа</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td align="center">
            Стартовая версия
            </td>
            <td align="center">
            2.97 ± 0.02
            </td>
        </tr>
        <tr>
            <td align="center">
            Предыдущая версия
            </td>
            <td align="center">
            1.004 ± 0.002
            </td>
        </tr>
    </tbody>
</table>

### Достаточно ли оптимизаций?

Последняя оптимизация принесла прирост на 0.4 процента относительно предыдущей версии, что означает, что на этом пора заканчивать, так как выгода от последующий оптимизаций не будет стоить потраченного времени.

## Обсуждение результатов

<table>
    <thead>
        <tr>
            <th align="center">Версия программы</th>
            <th align="center">Количество тактов, которое отработала программа, *10<sup>10</sup></th>
            <th align="center">Ускорение программы по сравнению с начальной версией</th>
            <th align="center">Ускорение программы по сравнению с предыдущей версией</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td align="center">
            Стартовая версия
            </td>
            <td align="center">
            9.65 ± 0.01
            </td>
            <td align="center">
            1
            </td>
            <td align="center">
            </td>
        </tr>
        <tr>
            <td align="center">
            Версия с SIMD
            </td>
            <td align="center">
            7.39 ± 0.07
            </td>
            <td align="center">
            1.31 ± 0.01
            </td>
            <td align="center">
            1.31 ± 0.01
            </td>
        </tr>
        <tr>
            <td align="center">
            Версия с переписанной на ассемблер функцией
            </td>
            <td align="center">
            3.27 ± 0.05
            </td>
            <td align="center">
            2.95 ± 0.04
            </td>
            <td align="center">
            2.26 ± 0.04
            </td>
        </tr>
        <tr>
            <td align="center">
            Версия с ассемблерной вставкой
            </td>
            <td align="center">
            3.25 ± 0.03
            </td>
            <td align="center">
            2.97 ± 0.02
            </td>
            <td align="center">
            1.004 ± 0.002
            </td>
        </tr>
        <tr>
            <td align="center">
            Все версии на одной диаграмме
            </td>
            <td align="center"  colspan = 3>
            <img src="data/result_with_StrCmp.png" alt="Фото">
            </td>
        </tr>
    </tbody>
</table>

## Выводы

В данной работе мы смогли применить три вида оптимизаций программы -

1. Использование **SIMD** интринсиков для работы со строками
2. Переписывание функции на язык ассемблера с целью оптимального распределения инструкций с учётом времени доступа в память
3. Использование ассемблерной вставки с учётом распределения данных по регистрам.

Суммарно все эти три оптимизации дали ускорение программы в 3 раза, что довольно много, если масштабировать нашу задачу.

### КПД

Рассчитаем **КПД** наших оптимизаций (*КПД = Коэффициент оптимизации / Количество ассемблерных строк кода*)

$КПД = \frac {2.97} {84} \cdot 1000 = 35.4 ± 0.2$

Таким образом, **КПД** получилось равным 35.4 ± 0.2.
