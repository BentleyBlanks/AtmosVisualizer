// create: true if it was server
function init(mqName, create, msgMaxNum, msgMaxSize)->[returnValue]

function uninit()->[returnValue]

// msgType
// 
function sendMessage(msgType, args0, args1...)->[returnValue]

// returnValue.success: false when there is no message in queue
function getMessage()->[returnValue, [msgType, msgArgs0, msgArgs1...]]

function isEmpty()->[returnValue, boolean]

function isFull()->[returnValue, boolean]

function getSize()->[returnValue, size]

