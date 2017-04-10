{
  "targets": [
    {
      "target_name": "a3MessageQueue",
      'type': "static_library",
      "sources": [ 
	      "a3MessageQueue.cpp", 
	      "a3MessageQueueIPC.cpp"
	  ],

	  'include_dirs': [ "./" ]
    },

    {	    
    	"target_name": "a3MQExports",
	    "sources": [ 
	    	"a3MQExports.cpp" 
	    ],
	    "dependencies": [ 
	    	"a3MessageQueue" 
	    ]
    }
  ]
}