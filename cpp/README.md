
Usar o epoll 

[] parser do arquivo de configuração
[] iniciar epoll
[] parse request
[] socket
[] parse response


Ajuda para implementar a parte mais complexa

Implementar um servidor web que aceita arquivos enviados, salva-os em um local específico e interage com um CGI é uma tarefa complexa que envolve muitos detalhes. Aqui está um esboço de alto nível de como você pode abordar isso:

Aceitando arquivos enviados: Você precisará analisar as solicitações HTTP POST que contêm um corpo de mensagem multipart/form-data. Isso geralmente é usado para enviar arquivos. Você precisará analisar o cabeçalho Content-Type para obter o tipo de mídia e os parâmetros, como o limite usado para separar as diferentes partes da mensagem. Em seguida, você pode ler o corpo da mensagem e separá-lo em suas partes componentes com base no limite.

Salvando arquivos enviados: Depois de ter o arquivo enviado, você pode salvá-lo em um local específico usando funções de E/S de arquivo padrão. O local específico pode ser configurado em algum lugar em seu servidor, como um arquivo de configuração.

Interagindo com um CGI: Para interagir com um CGI, você precisará criar um novo processo e executar o script CGI nesse processo. Você pode usar a função fork() para criar um novo processo e a função exec() para executar o script CGI. Você precisará configurar o ambiente do processo filho para incluir todas as variáveis de ambiente CGI necessárias, como REQUEST_METHOD, CONTENT_TYPE, CONTENT_LENGTH, QUERY_STRING, SCRIPT_NAME, PATH_INFO, etc. Você também precisará redirecionar a entrada padrão do processo filho para o corpo da solicitação HTTP e a saída padrão do processo filho de volta para o cliente.

Lidando com solicitações e respostas em pedaços: Para solicitações em pedaços, você precisará ler cada pedaço, decodificá-lo e juntar todos os pedaços para formar o corpo completo da solicitação. Para respostas em pedaços do CGI, você precisará ler a saída do CGI até EOF e enviá-la de volta para o cliente.

Executando o CGI no diretório correto: Para garantir que o CGI seja executado no diretório correto para acesso a arquivos de caminho relativo, você pode usar a função chdir() para alterar o diretório de trabalho do processo filho para o diretório desejado antes de executar o CGI.

Trabalhando com diferentes CGIs: Para fazer seu servidor funcionar com diferentes CGIs (como PHP, Python, etc.), você pode configurar o caminho para o interpretador CGI em algum lugar em seu servidor, como um arquivo de configuração. Então, quando você executa o CGI, você usa esse caminho para o interpretador CGI.

Implementar todas essas funcionalidades em um servidor web é uma tarefa grande e complexa que vai além do escopo desta resposta. Eu recomendaria dividir a tarefa em partes menores e implementar cada parte uma de cada vez. Você também pode querer considerar o uso de uma biblioteca de servidor web existente que já fornece muitas dessas funcionalidades.