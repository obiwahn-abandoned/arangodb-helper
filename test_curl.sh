#!/bin/bash

case $1 in
    js)
        echo "JavaScript used for Request"
        #curl -X GET http://127.0.0.1:8529/_admin/time--dump - --header "Accept:application/x-velocypack"
        curl -X GET http://127.0.0.1:8529/_admin/time--dump --header "Accept:application/x-velocypack"
        echo
    ;;
    *)
        echo "default case"
        curl -XPOST http://localhost:8529/_api/document?collection=test -d '{ "foo" : "bar"  }' --header "Accept: application/x-velocypack"
        echo
    ;;
esac
