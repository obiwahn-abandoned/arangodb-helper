#break arangodb::rest::VppCommTask::readChunkHeader
#commands
#printf "%s\n", "##################################################"
#finish
#end
#
#break VppRequest.cpp:79
#commands
#printf "%s\n", "##################################################"
#print _message
#cont
#end
#
#break arangodb::VppRequest::headers
#commands
#printf "%s\n", "HEADERS##################################################"
#end
#
#break arangodb::VPackMessage::VPackMessage
#commands
#printf "%s\n", "MESSAGE##################################################"
#end

catch throw
#disable 1

break arangodb::rest::VppCommTask::processRead
#disable 2

#break VppCommTask.cpp:325
#commands
#printf "%s\n", "## VppCommTask::processRead ####################################"
#print *_request
#cont
#end

b SocketTask.cpp:256
commands
printf "%s\n", "## SocketTask::setWriteBuffer ####################################"
printf "%s\n", "_writeBuffer->_buffer._buffer"
print _writeBuffer->_buffer._buffer
cont
end

break VppCommTask.cpp:361

info breakpoints

run
