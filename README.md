# MRND_MazeSolver
Amazing client and server implementation which can solve maze problem.


Client: 
  Sends a maze problem written in a file to server using sockets.
  recieves the solution and writes back to same file.

Server:
  recieves the maze problem, calculates the path from (a, b) to (c, d)
  Sends back the answer to client who requested.
