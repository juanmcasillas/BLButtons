///////////////////////////////////////////////////////////////////////////////
// Dispatcher callback ////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef __CALLBACK_H__
#define __CALLBACK_H__

void callbackHandler(AsyncWebServerRequest *request) {

    // return built status (JSON)
	if (request->url() == "/blbuttons/status.json") {
        String ret = BLBUTTONS.GetStatus(request);
        //DEBUGLOG("callbackHandler:/status.json: (%s)\n",ret.c_str());
        //DEBUGLOG("callbackHandler:/status.json: (%s)\n","...");
        request->send(200, "application/json", ret);
        return;
	}    

    // Save partial config (JSON)
	if (request->url() == "/blbuttons/save.json") {
        bool error = false;
        String ret = BLBUTTONS.SaveConfig(request, &error);
        //DEBUGLOG("callbackHandler:/save.json: (%s)\n",ret.c_str());
        //DEBUGLOG("callbackHandler:/save.json: (%s)\n","...");
        if (!error)
            request->send(200, "application/json", ret);
        else
            request->send(500, "application/json", ret);
        return;
	}      



}

#endif