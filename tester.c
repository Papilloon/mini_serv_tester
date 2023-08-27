/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tester.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrienmori <adrienmori@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/27 16:15:59 by adrienmori        #+#    #+#             */
/*   Updated: 2023/08/27 17:54:19 by adrienmori       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>

int	clients[1024];

int	open_socket(int id, int port);
int	close_socket(int id);
int	send_to_socket(int id, char *buff);

void	send_to_serv(int socket, char *buff)
{
	send(socket, buff, strlen(buff), MSG_DONTWAIT);
}

int	tester(int port)
{
	usleep(100000);

	open_socket(1, port); usleep(10000);
	open_socket(2, port); usleep(10000);
	open_socket(3, port); usleep(10000);
	open_socket(4, port); usleep(10000);
	open_socket(5, port); usleep(10000);

	send_to_socket(1, "Je suis client 1\n"); usleep(10000);
	send_to_socket(2, "Je suis client 2\n"); usleep(10000);
	send_to_socket(3, "Je suis client 3\n"); usleep(10000);
	send_to_socket(4, "Je suis client 4\n"); usleep(10000);
	send_to_socket(5, "Je suis client 5\n"); usleep(10000);

	send_to_socket(1, "Je n'ai pas de retour a la ligne");
	send_to_socket(1, " Et j'en ajoute un ici\n");
	send_to_socket(1, "J'ai\nPlusieurs\nretour\na\nla\nligne\n");  usleep(10000);

	send_to_socket(1, "J'ecris sans endline avant de me deconnecter");  usleep(10000);

	close_socket(1);  usleep(10000);
	close_socket(3);  usleep(10000);
	close_socket(5);  usleep(10000);

	open_socket(6, port);  usleep(10000);
	open_socket(7, port);  usleep(10000);

	send_to_socket(6, "Je suis client 6\n");  usleep(10000);
	send_to_socket(7, "Je suis client 7\n");  usleep(10000);

	send_to_socket(2, "Je suis client 2 et je fonctionne toujours\n");  usleep(10000);
	send_to_socket(4, "Je suis client 4 et je fonctionne toujours\n");  usleep(10000);

	send_to_socket(6, "Je suis un texte de 10000 characteres pour tester le buffer : ");  usleep(10000);
	for (int i = 0;  i < 278; i++)
		send_to_socket(6, "1234567890abcdefghijklmnopqrstuvwxyz");
	send_to_socket(6, "\n");  usleep(10000);

	close_socket(2);  usleep(10000);
	close_socket(4);  usleep(10000);
	close_socket(6);  usleep(10000);
	close_socket(7);  usleep(10000);
	usleep(100000);
	return (0);
}

int	main(int argc, char **argv)
{
	if (argc < 2)
		return (printf("erreur: ./tester [port]\n"), 1);

	int file = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644); // Ouvrir ou créer le fichier en écriture
	if (file == -1)
		return (printf("erreur: lors de l'ouverture du fichier"), 1);

	int	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0)
		return (printf("erreur: connexion impossible\n"), 1);

	struct sockaddr_in	addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[1]));
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	if (connect(server_socket, (const struct sockaddr *)&addr, sizeof(addr)) == -1)
		return (printf("erreur: connect impossible\n"), 1);

	bzero(clients, sizeof(clients));

	int pid = fork();

	if (!pid)
	{
		tester(atoi(argv[1]));
		kill(pid, SIGKILL);
//		close(server_socket);
		close(file);
		exit(0);
	}
	else
	{
		char buffer[1024];
		ssize_t bytesRead;

		while ((bytesRead = recv(server_socket, buffer, sizeof(buffer), 0)) > 0) {
			write(file, buffer, bytesRead);
		}

		if (bytesRead < 0) {
			perror("Erreur lors de la réception des données");
		}

		close(server_socket);
		exit(0);
	}
}

int	open_socket(int id, int port)
{
	int					sockfd;
	struct sockaddr_in	servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		return (-1);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	if (connect(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
		return (-1);

	clients[id] = sockfd;
	return (0);
}

int	close_socket(int id)
{
	close(clients[id]);
	clients[id] = 0;
	return (0);
}

int	send_to_socket(int id, char *buff)
{
	send(clients[id], buff, strlen(buff), 0);
	return (0);
}
