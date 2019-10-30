# Драйвер и клиентское ПО для USB считывателя RFID-меток Micro-EM (Микро-ЭМ)

## Что это?

`driver` — модуль ядра Linux для USB-устройства `c251:130a`, это считыватель RFID-меток.  
Создает псевдо-устройства `/dev/microemN`, где вместо N цифра. Первый подключенный USB-сканер `c251:130a` будет иметь адрес `/dev/microem0`.

`scanner` — это программа дял считывание RFID-меток.

Изначальное происхождение кода неизвестно. ВСе, что было импортировано в первом коммите, написано не мной. Если код нарушает ваши авторские права, сообщите об этом. Вопросы вызывают куски какого-то SDK в `scanner/`. Общее качество кода не очень.

`scanner/microemscan.service` — сервис systemd для запуска программы `microemscan` в режиме демона.

## Сборка

Для сборки в папках `driver` и `scanner` запускать `make`, для установки запускать `make install`. Для установки сервиса systemd выполните `make install-systemd`.

## Работа с сервисом systemd

* `systemctl enable microemscan` - добавить в автозагрузку
* `systemctll disable microemscan` - убрать из автозагрузки
* `systemctl status microemscan` - узнать состояние
* `journalctl -b 0 -u microemscan` - узнать лог работы

## Установка на ROSA Linux

Для установки на ROSA достаточно из репозитория поставить пакет `microemscan` (`sudo urpmi microemscan`). Пакет `dkms-microemscan-rfid` с модулем ядра (драйвером) подтянется как зависимость.

Исходники пакета здесь: [https://abf.io/import/microem-rfid-linux](https://abf.io/import/microem-rfid-linux)
