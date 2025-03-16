1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The client reads data in a loop until it receives the EOF character (0x04), signaling the end of the message. Techniques to handle partial reads include buffered reading, checking for 0x04, and ensuring null-terminated (\0) commands.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

Each command is null-terminated (\0), and server responses end with EOF (0x04). Without clear boundaries, commands may merge or be split, causing incorrect execution or waiting indefinitely for input.

3. Describe the general differences between stateful and stateless protocols.

 - Stateful: Remembers previous interactions (e.g., TCP, SSH, HTTP with sessions). Requires more memory and session tracking.

 - Stateless: Treats each request independently (e.g., UDP, HTTP without sessions, DNS). Faster and scales better.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

UDP is used for low-latency, real-time communication (e.g., video streaming, VoIP, gaming) because it has no connection overhead and is faster than TCP. It also supports broadcast and multicast.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The socket API allows applications to use TCP and UDP. Key functions include:
 - socket() – Create a socket.
 - bind() – Bind to an address.
 - listen() – Wait for connections (TCP).
 - accept() – Accept a connection (TCP).
 - send() / recv() – Send and receive data.
Sockets provide a standard interface for network programming across operating systems.


