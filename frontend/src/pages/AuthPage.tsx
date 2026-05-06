import React, { useState, useEffect } from 'react';
import { useLocation } from 'react-router-dom';
import { motion } from 'framer-motion';
import LoginForm from '../components/auth/LoginForm';
import RegisterForm from '../components/auth/RegisterForm';
import './AuthPage.css';

const AuthPage: React.FC = () => {
  const location = useLocation();
  const [isFlipped, setIsFlipped] = useState(location.pathname === '/register');

  useEffect(() => {
    setIsFlipped(location.pathname === '/register');
  }, [location.pathname]);

  return (
    <div className="min-h-screen bg-[#0a0f1e] flex items-center justify-center p-5 relative overflow-hidden">
      {/* Background Orbs */}
      <div className="fixed inset-0 overflow-hidden pointer-events-none z-0">
        <div className="absolute w-[400px] h-[400px] bg-blue-600 rounded-full filter blur-[100px] opacity-20 top-20 left-10 animate-pulse" />
        <div className="absolute w-[350px] h-[350px] bg-purple-600 rounded-full filter blur-[100px] opacity-20 bottom-20 right-10 animate-pulse delay-700" />
        <div className="absolute w-[300px] h-[300px] bg-indigo-500 rounded-full filter blur-[100px] opacity-10 top-1/3 right-1/4 animate-pulse delay-1000" />
      </div>

      <div className="relative z-10 w-full max-w-5xl bg-[#0f172a]/80 backdrop-blur-2xl rounded-3xl border border-white/10 shadow-2xl overflow-hidden flex flex-col md:flex-row">
        
        {/* Left Side: Image Banner */}
        <div className="hidden md:block md:w-1/2 relative">
          <img 
            src="https://images.unsplash.com/photo-1515187029135-18ee286d815b?auto=format&fit=crop&w=800&q=80" 
            alt="Tech Conference" 
            className="absolute inset-0 w-full h-full object-cover"
          />
          <div className="absolute inset-0 bg-gradient-to-t from-[#0f172a] via-[#0f172a]/40 to-transparent" />
          <div className="absolute bottom-0 left-0 p-10">
            <h2 className="text-3xl font-extrabold text-white mb-3">Welcome to the Future of Research</h2>
            <p className="text-slate-300">Join the premier platform for academic collaboration, peer review, and conference management.</p>
          </div>
        </div>

        {/* Right Side: Auth Forms */}
        <div className="w-full md:w-1/2 p-6 sm:p-10 flex flex-col justify-center items-center relative">
          <div className="w-full max-w-md">
            <div className={`flip-card ${isFlipped ? 'flipped' : ''} w-full`}>
              <div className="flip-card-inner">
                {/* Front: Login */}
                <div className="flip-front">
                  <LoginForm onSwitch={() => setIsFlipped(true)} />
                </div>

                {/* Back: Register */}
                <div className="flip-back">
                  <RegisterForm onSwitch={() => setIsFlipped(false)} />
                </div>
              </div>
            </div>

            <motion.p 
              initial={{ opacity: 0 }}
              animate={{ opacity: 1 }}
              transition={{ delay: 1 }}
              className="text-center text-slate-500 text-xs mt-8"
            >
              🔐 Secure, encrypted connection to Conference API
            </motion.p>
          </div>
        </div>
      </div>
    </div>
  );
};

export default AuthPage;
