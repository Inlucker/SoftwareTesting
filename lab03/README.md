#  Сравнение СУБД

- Postgresql 
- MySQL
- SQLite


### _**Для каждого параметра представлены 3 графика -- 10 строк, 100 строк, 500 строк в таблице.**_


## Insert
![Insert](reports/graphs/insert.png)


## Update
![Insert](reports/graphs/update.png)



## Get all
![Insert](reports/graphs/get_all.png)


### Get by login
![Insert](reports/graphs/get_by_login.png)


## Delete
![Insert](reports/graphs/delete.png)


## Выводы
Postgresql выигрывает по времени работы по всех параметрам, кроме получения записи по значению ключа. 
Так как основными операциями в ПО, для которого проводится исследование, являются получение всех записей и изменение записи, выбор postgresql является оправданным.
Разница во времени работы между MySQL и SQLite не столь значительна, но в по пользу MySQL.

При этом SQLite быстрее конкурентов выполняет получение записи по значению. 
В прилежениях, где эта операция является самой частой, эта СУБД может быть более оптимальным решением.