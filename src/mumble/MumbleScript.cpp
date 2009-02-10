/* Copyright (C) 2005-2009, Thorvald Natvig <thorvald@natvig.com>

   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   - Neither the name of the Mumble Developers nor the names of its
     contributors may be used to endorse or promote products derived from this
     software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "MumbleScript.h"
#include "Global.h"
#include "Log.h"
#include "ServerHandler.h"

ScriptPlayer::ScriptPlayer(ClientPlayer *p) : QObject(p) {
}

QString ScriptPlayer::getName() const {
	ClientPlayer *cp = qobject_cast<ClientPlayer *>(parent());
	return cp->qsName;
}

unsigned int ScriptPlayer::getSession() const {
	ClientPlayer *cp = qobject_cast<ClientPlayer *>(parent());
	return cp->uiSession;
}

int ScriptPlayer::getId() const {
	ClientPlayer *cp = qobject_cast<ClientPlayer *>(parent());
	return cp->iId;
}

QScriptValue ScriptPlayer::getChannel() const {
	ClientPlayer *cp = qobject_cast<ClientPlayer *>(parent());
	Channel *c = cp->cChannel;
	QScriptEngine *qse = engine();
	MumbleScript *ms = qobject_cast<MumbleScript *>(qse->parent());
	return qse->newQObject(ms->qmChannels.value(Channel::get(0)), QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject);
}

bool ScriptPlayer::isMute() const {
	ClientPlayer *cp = qobject_cast<ClientPlayer *>(parent());
	return cp->bMute || cp->bDeaf;
}

bool ScriptPlayer::isDeaf() const {
	ClientPlayer *cp = qobject_cast<ClientPlayer *>(parent());
	return cp->bDeaf;
}

void ScriptPlayer::setMute(bool m) {
	ClientPlayer *cp = qobject_cast<ClientPlayer *>(parent());
	qWarning("Want to set mute to %d", m);
}

void ScriptPlayer::setDeaf(bool m) {
	ClientPlayer *cp = qobject_cast<ClientPlayer *>(parent());
	qWarning("Want to set deaf to %d", m);
}

void ScriptPlayer::setChannel(const QScriptValue &sv) {
	ClientPlayer *cp = qobject_cast<ClientPlayer *>(parent());
	ScriptChannel *sc = qobject_cast<ScriptChannel *>(sv.toQObject());
	if (sc) {
		Channel *c = qobject_cast<Channel *>(sc->parent());
		qWarning("Want to set channel to %d", c->iId);
	} else {
		context()->throwError(QScriptContext::ReferenceError, QLatin1String("argument is not a channel"));
	}
}

void ScriptPlayer::sendMessage(const QString &msg) {
	ClientPlayer *cp = qobject_cast<ClientPlayer *>(parent());
	qWarning("Want to send msg %s", qPrintable(msg));
}

ScriptChannel::ScriptChannel(Channel *c) : QObject(c) {
}

QString ScriptChannel::getName() const {
	Channel *c = qobject_cast<Channel *>(parent());
	return c->qsName;
}

int ScriptChannel::getId() const {
	Channel *c = qobject_cast<Channel *>(parent());
	return c->iId;
}

int ScriptChannel::getParent() const {
	Channel *c = qobject_cast<Channel *>(parent());
	return c->cParent ? c->cParent->iId : -1;
}

void ScriptChannel::setName(const QString &name) {
	Channel *c = qobject_cast<Channel *>(parent());
	qWarning() << "ScriptChannel::setName" << name;
}

void ScriptChannel::setParent(int id) {
	Channel *c = qobject_cast<Channel *>(parent());
	qWarning() << "ScriptChannel::setParent" << id;
}

void ScriptChannel::sendMessage(const QString &msg, bool tree) {
	Channel *c = qobject_cast<Channel *>(parent());
	qWarning() << "ScriptChannel::sendMessage" << msg << tree;
}

QScriptValue ScriptChannel::getPlayers() const {
	Channel *c = qobject_cast<Channel *>(parent());
	QScriptEngine *qse = engine();
	MumbleScript *ms = qobject_cast<MumbleScript *>(qse->parent());

	QScriptValue a = engine()->newArray(c->qlPlayers.count());

	quint32 idx = 0;

	foreach(Player *p, c->qlPlayers) {
		ClientPlayer *cp = static_cast<ClientPlayer *>(p);
		QScriptValue sv = qse->newQObject(ms->qmPlayers.value(cp), QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject);
		a.setProperty(idx, sv);
		idx++;
	}
	return a;
}

QScriptValue ScriptChannel::getChildren() const {
	Channel *c = qobject_cast<Channel *>(parent());
	QScriptEngine *qse = engine();
	MumbleScript *ms = qobject_cast<MumbleScript *>(qse->parent());

	QScriptValue a = engine()->newArray(c->qlChannels.count());

	quint32 idx = 0;

	foreach(Channel *child, c->qlChannels) {
		QScriptValue sv = qse->newQObject(ms->qmChannels.value(child), QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject);
		a.setProperty(idx, sv);
		idx++;
	}
	return a;
}

ScriptServer::ScriptServer(QObject *p) : QObject(p) {
}

QScriptValue ScriptServer::getRoot() const {
	qWarning("getRoot())");
	Channel *c = qobject_cast<Channel *>(parent());
	QScriptEngine *qse = engine();
	MumbleScript *ms = qobject_cast<MumbleScript *>(qse->parent());
	return qse->newQObject(ms->qmChannels.value(Channel::get(0)), QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject);
}

QScriptValue ScriptServer::loadUi(const QString &name) {
	QUiLoader *ui = new QUiLoader();

	qWarning() << "ScriptServer::loadUi" << name;
	return QScriptValue();
}

void ScriptServer::sendMsg(const QString &keyword, const QStringList &args) {
	qWarning() << "ScriptServer::sendMsg" << keyword << args;
}

QScriptValue mumbleScriptPrint(QScriptContext *context, QScriptEngine *engine) {
	QString result;

	for (int i = 0; i < context->argumentCount(); ++i) {
		if (i > 0)
			result.append(QLatin1String(" "));
		result.append(context->argument(i).toString());
	}

	qWarning() << result;
	g.l->log(Log::ScriptError, result, MumbleScript::tr("Script Print"));

	return engine->undefinedValue();
}

MumbleScript::MumbleScript(MumbleScripts *p) : QObject(p) {
	qseEngine = new QScriptEngine(this);
	ssServer = new ScriptServer(qseEngine);
	bLocal = true;

	qseEngine->globalObject().setProperty(QLatin1String("server"), qseEngine->newQObject(ssServer, QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject), QScriptValue::ReadOnly | QScriptValue::Undeletable);
	qseEngine->globalObject().setProperty(QLatin1String("print"), qseEngine->newFunction(mumbleScriptPrint), QScriptValue::ReadOnly | QScriptValue::Undeletable);

	connect(qseEngine, SIGNAL(signalHandlerException(const QScriptValue &)), this, SLOT(errorHandler(const QScriptValue &)));

	connect(g.sh, SIGNAL(disconnected(QString)), ssServer, SIGNAL(disconnected(QString)));
}

MumbleScript::~MumbleScript() {
	foreach(ScriptPlayer *sp, qmPlayers)
		delete sp;
}

void MumbleScript::load(const QString &) {
}

void MumbleScript::evaluate(const QString &code) {
	qseEngine->evaluate(code);
	if (qseEngine->hasUncaughtException())
		errorHandler(qseEngine->uncaughtException());
}

void MumbleScript::errorHandler(const QScriptValue &exception) {
	g.l->log(Log::ScriptError, exception.toString() + qseEngine->uncaughtExceptionBacktrace().join(QLatin1String("\n")), tr("Script Error"));
	g.ms->qlScripts.removeAll(this);
	delete this;
}

void MumbleScript::addPlayer(ClientPlayer *p) {
	connect(p, SIGNAL(destroyed(QObject *)), this, SLOT(playerDeleted(QObject *)));
	ScriptPlayer *sp = new ScriptPlayer(p);
	qmPlayers.insert(p, sp);
	emit ssServer->newPlayer(qseEngine->newQObject(sp, QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject));
}

void MumbleScript::movePlayer(ClientPlayer *p) {
	ScriptPlayer *sp = qmPlayers.value(p);
	emit sp->moved();
}

void MumbleScript::addChannel(Channel *c) {
	connect(c, SIGNAL(destroyed(QObject *)), this, SLOT(channelDeleted(QObject *)));
	ScriptChannel *sc = new ScriptChannel(c);
	qmChannels.insert(c, sc);
	emit ssServer->newChannel(qseEngine->newQObject(sc, QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject));
}

void MumbleScript::moveChannel(Channel *c) {
	ScriptChannel *sc = qmChannels.value(c);
	emit sc->moved();
}

void MumbleScript::playerDeleted(QObject *obj) {
	ClientPlayer *cp=qobject_cast<ClientPlayer *>(obj);
	qmPlayers.remove(cp);
}

void MumbleScript::channelDeleted(QObject *obj) {
	Channel *c=qobject_cast<Channel *>(obj);
	qmChannels.remove(c);
}

void MumbleScript::connected() {
	emit ssServer->connected();
}

MumbleScripts::MumbleScripts(QObject *p) : QObject(p) {
}

MumbleScripts::~MumbleScripts() {
	foreach(MumbleScript *ms, qlScripts)
		delete ms;
}

void MumbleScripts::createEvaluate(const QString &qs) {
	MumbleScript *ms = new MumbleScript(this);
	qlScripts << ms;
	ms->evaluate(qs);
}

void MumbleScripts::addPlayer(ClientPlayer *p) {
	foreach(MumbleScript *ms, qlScripts)
		ms->addPlayer(p);
}

void MumbleScripts::movePlayer(ClientPlayer *p) {
	foreach(MumbleScript *ms, qlScripts)
		ms->movePlayer(p);
}

void MumbleScripts::addChannel(Channel *c) {
	foreach(MumbleScript *ms, qlScripts)
		ms->addChannel(c);
}

void MumbleScripts::moveChannel(Channel *c) {
	foreach(MumbleScript *ms, qlScripts)
		ms->moveChannel(c);
}

void MumbleScripts::connected() {
	foreach(MumbleScript *ms, qlScripts)
		ms->connected();
}
