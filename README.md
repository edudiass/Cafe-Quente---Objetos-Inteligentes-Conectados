# Café Quente Objetos Inteligentes Conectados
## Controlando sua cafeteira de onde e quando quiser.

“Café Quente” Nosso projeto utilizará um circuito de automação controlado com módulo relé 2 canais de 5V e uma placa NodeMcu 
integrada com um MQTT Dash, que receberá as informações de rede e acionará as portas do relé, e assim, controlar a cafe-teira 
e uma bomba submersível, que será utilizada para adicionar a água necessária para que o nosso café seja feito; de qualquer lugar 
e qualquer hora; e também será pos-sível monitorar a temperatura desse café com a utilização de um sensor Ds18b20 wa-terproof. 
Para que isso tudo seja possível, basta o usuário possuir um celular com aces-so à internet e conectar-se à página de controle.

Para nosso projeto, foram utilizados os seguintes componentes, Placa Wifi ESP8266 que recebe o código desenvolvido no Arduino IDE, 
onde será estabelecidas todas as configurações para acesso ao Broker, que no será o Cloud MQTT. Além desse componente, foram utilizados 
um módulo Relé 2 canais, que atuara como um interruptor de energia da cafeteira, e de uma bomba submersiva que será utilizada para 
adicionar água na cafeteira para preparo do café, também será utilizado um sensor Ds18b20 para monitoramento da temperatura. Tudo isso
você pode acompanhar com mais detalhes através desse link: https://www.youtube.com/watch?v=gntIfP7dR3Y&feature=youtu.be

