(function(){
    'use strict'
    var fs = require('fs')
    var db = require('internal').db
    var time = require('internal').time

    function load_data(directory, col){
        print("get files in: " + directory  )
        var files = fs.list(directory)
        if (files.length == 0){
            print("no files to load");
            col = null;
            return col;
        }
        files.sort()
        print("OBI" + col)
        var buffer;
        var uid = 0;
        var start = time()
        files.forEach( function(file){
            print("reading: " + file)
            buffer=fs.readFileSync(directory + fs.pathSeparator + file)
            var lines = buffer.toString().split('\n')

            lines.forEach(function(line){
                col.save({ "_key" : uid.toString()
                         , "data" : line
                         });
                uid += 1;
            })
        })
        print(" reading files took: " + (time() - start).toString() + " seconds")
        return col
    }

    function get_collection(path, name, drop){
        print("enter create collection")
        if (drop){
            print("drop")
            db._drop(name)
        }
        var col = db[name];
        if (col) {
            print("returning existing collection")
	    print("loading collection")
            var start = time()
            col.load()
            print(" loading took " + (time() - start).toString() + " seconds")
            return col;
        }
        print("create new")
        col = db._create(name)
        col = load_data(path, col)
        return col
    }

    function run_test(col){
        print("starting test")
        var start = time()

        // //very slow
        // var biggest_key = function(col){
        //     db._query(
        //     ` FOR doc IN @@col
        //         let key = to_number(doc._key)
        //         SORT key
        //         LIMIT 1
        //         RETURN key
        //     `, { "@col" : col.name() }
        //     )
        // }(col)


        print(col.properties());
        var count = col.count();
        print("there are " + count.toString() + " elements in the collection");

        function next_key_max_all(current, count){
            return (( current + 47 ) % count)
        }
        
	function next_key_max_1(current, count){
            return (( current + 47 * 100 ) % count)
        }

	function next_key_lcg(current){
		//https://en.wikipedia.org/wiki/Linear_congruential_generator
		return (48271 * current % 2147483647);
	}

	function create_yield_next(initial, count){
		var current = initial;
		return (function(){
			current = next_key_lcg(current) % count;
			return current % count;
		})
	}

	//var yield_next = create_yield_next(1,count)
        var key = 1;

        var test_count = count / 10;
        print(test_count)
        for(var i = 0; i < test_count; ++i){
            var doc = col.document(key.toString())
	    if(i < 10){
		print( key.toString() + " -- " + doc["data"] );
	    }
	    if (i % 100000 == 0){print(i)}
            key = next_key_max_all(key,count);
	    //print(key);
	    //key = yield_next()
	    
        }

        return (time() - start)
    }

    function main(path, name){
        var col = get_collection(path, name, false)
        if(!col){
            print("no collection")
            return 1
	}
        
        var duration = run_test(col)

        print("total time taken: " + duration.toString())
        //print(col.figures())
        return 0
    }

    return main;
})()
