# Powercenter Tests

### 1 UDP/TCP

| Was wurde getestet? | Wie wurde getestet? | Erwartetes Ergebnis | Tatsächliches Ergebnis
| ----------- | ----------- | ----------- | ----------- |
| HTTP Anfragen | Skriptbasiert werden die unterstützten Fehlercodes abgeprüft| Die Fehlercodes 200, 400, 403, 404 und 405 werden korrekt zurückgegeben | Die Fehlercodes 200, 400, 403, 404 und 405 werden korrekt zurückgegeben. Bei 404 kommt Serverseitig zudem eine Fehlermeldung, dass die Datei nicht gefunden wurde. |
| JSON   | Falsch formatierte Daten werden an Zentrale gesendet | Eine dementsprechende Meldung wird in Logs angezeigt und das System stürzt nicht ab | Die Fehlermeldung lautet: "ERROR: did not receive JSON-Formatted data!" |
| Ausfall eines Participants | Das dementsprechende Container wird gestoppt | Das System stürzt nicht ab | Das System stürzt nicht ab. 3 andere Participants laufen weiter |
| Leeres Paket | Das fehlerhafte Paket wird zur Zentrale gesendet | Das System merkt das leere Paket und stürzt nicht ab |Das System merkt das leere Paket und stürzt nicht ab. In den Logs wird die Fehlermeldung angezeigt. |
| Übertragungszeit eines Pakets | Mithilfe von Timestamps im Participant und Powercenter (ms werden betrachtet) | Die Überstragungszeit beträgt weniger als 2ms. | 0-1ms pro Aufruf |
| Antwortzeiten unter Last (Testseite) | Kleines C++-Programm, welches 1000 HTTP-Anfragen stellt. Die Anfragen gehen alle auf die Test-Seite. Alle anderen Container wurden gestoppt.| ca. 5-50ms pro Aufruf | 3.2-3.6ms pro Aufruf |
| Antwortzeiten unter Last (Datei) | Kleines C++-Programm, welches 1000 HTTP-Anfragen stellt. Die Anfragen gehen alle auf die Datei "history". Alle anderen Container wurden gestoppt.| ca. 10-100ms pro Aufruf | 5-30ms pro Aufruf |

### 2 RPC

| Was wurde getestet? | Wie wurde getestet? | Erwartetes Ergebnis | Tatsächliches Ergebnis
| ----------- | ----------- | ----------- | ----------- |
| Externe Anfragen | Eine RPC-Anfrage wird geschickt | Die Anfrage wird bearbeitet | Die Anfrage wird bearbeitet und gestückelt (nach RPC-Übergabeparametern) zurückgegeben. |
| REST-API | Die maximal produzierte Strommenge oder Wert 0 bei der Deaktivierung werden eingegeben. | Der eingegebene Wert wird eingelesen und festgelegt. | Der eingegebene Wert wird eingelesen und festgelegt, wenn man für einen bestimmten Erzeuger/Verbraucher die Strommenge definiert. |
| Bearbeitung von RPC | Es wird eine RPC-Anfrage mit dem neuen Wert an Participant geschickt. | Der Wert mit der Strommenge wird empfangen und demensprechend angepasst. | Der ansprechende Participant liest den neuen Wert ein und passt seine Strommenge an. |
| Dauer der Anfragen | Mithilfe von Timestamps im Code vor und nach dem Funktionsaufruf. | ca. 15-20ms pro Aufruf| ca. 13-70ms pro Aufruf |

### 3 MoM mittels MQTT

| Was wurde getestet? | Wie wurde getestet? | Erwartetes Ergebnis | Tatsächliches Ergebnis
| ----------- | ----------- | ----------- | ----------- |
| ... | ....| ...| ... |
