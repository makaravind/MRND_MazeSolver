# MRND_MazeSolver
Amazing client and server implementation which can solve maze problem.


Client: 

  1) Sends a maze problem written in a file to server using sockets.
  
  2) recieves the solution and writes back to same file.

Server:

  1) recieves the maze problem, calculates the path from (a, b) to (c, d)
  
  2) Sends back the answer to client who requested.
