import React, { useState, useEffect } from 'react';
import { useLocation } from 'react-router-dom';
import { motion } from 'framer-motion';
import LoginForm from '../components/auth/LoginForm';
import RegisterForm from '../components/auth/RegisterForm';
import DynamicBackground from '../components/auth/DynamicBackground';
import './AuthPage.css';

const AuthPage: React.FC = () => {
  const location = useLocation();
  const [isFlipped, setIsFlipped] = useState(location.pathname === '/register');

  useEffect(() => {
    setIsFlipped(location.pathname === '/register');
  }, [location.pathname]);

  return (
    <div className="min-h-screen w-full flex items-center justify-center p-4 md:p-8 bg-[#f8fafc]">
      {/* Background blobs for depth */}
      <div className="fixed top-0 left-0 w-96 h-96 bg-blue-100 rounded-full blur-[100px] opacity-50 -translate-x-1/2 -translate-y-1/2" />
      <div className="fixed bottom-0 right-0 w-[500px] h-[500px] bg-indigo-50 rounded-full blur-[120px] opacity-60 translate-x-1/3 translate-y-1/3" />

      <div className="relative z-10 w-full max-w-6xl bg-white/40 backdrop-blur-3xl rounded-[2.5rem] border border-white shadow-[0_20px_50px_rgba(0,0,0,0.05)] overflow-hidden flex flex-col md:flex-row h-full max-h-[90vh]">
        
        {/* Left Side: Procedural Dynamic Banner */}
        <div className="hidden md:block md:w-5/12 relative group overflow-hidden bg-slate-50 border-r border-slate-100">
          <DynamicBackground />
          
          {/* Visual Overlays */}
          <div className="absolute inset-0 bg-gradient-to-t from-white/80 via-transparent to-transparent" />
          <div className="absolute inset-0 bg-blue-500/5 pointer-events-none" />
          
          {/* Logo Area */}
          <motion.div 
            initial={{ opacity: 0, y: -20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.5, duration: 0.8 }}
            className="absolute top-12 left-12 flex items-center gap-4 z-20"
          >
            <div className="relative">
              <div className="absolute inset-0 bg-blue-500 rounded-2xl blur-2xl opacity-10 animate-pulse" />
              <div className="w-14 h-14 bg-white border border-slate-100 rounded-2xl flex items-center justify-center relative shadow-sm group">
                <motion.img 
                  src="/assets/logo.png" 
                  alt="Logo" 
                  className="w-10 h-10 relative z-10"
                  animate={{ 
                    rotate: [0, 5, 0, -5, 0],
                    scale: [1, 1.05, 1] 
                  }}
                  transition={{ duration: 6, repeat: Infinity, ease: "easeInOut" }}
                />
              </div>
            </div>
            <div>
              <h1 className="text-2xl font-black tracking-tight text-slate-900 leading-none">CMS</h1>
              <p className="text-[10px] text-blue-600 font-bold tracking-[0.3em] uppercase mt-1">Intelligence</p>
            </div>
          </motion.div>

          <div className="absolute bottom-0 left-0 p-12 z-20 w-full">
            <motion.div
              initial={{ opacity: 0, x: -30 }}
              animate={{ opacity: 1, x: 0 }}
              transition={{ delay: 0.8, duration: 0.8 }}
            >
              <div className="inline-block px-3 py-1 bg-blue-50 border border-blue-100 rounded-full mb-6">
                <p className="text-blue-600 text-[10px] font-bold tracking-widest uppercase">Research Excellence</p>
              </div>
              <h2 className="text-5xl font-black text-slate-900 mb-4 tracking-tight leading-[1.1]">
                Revolutionizing <br />
                <span className="text-transparent bg-clip-text bg-gradient-to-r from-blue-600 to-indigo-600">Academic</span> <br />
                Flow
              </h2>
              <div className="w-20 h-1.5 bg-gradient-to-r from-blue-500 to-indigo-600 mb-6 rounded-full" />
              <p className="text-slate-500 max-w-xs text-sm leading-relaxed font-medium">
                The world's most advanced platform for conference management and review.
              </p>
            </motion.div>
          </div>

          {/* Decorative Corner Element */}
          <div className="absolute top-0 right-0 p-10 opacity-40 pointer-events-none">
            <div className="w-32 h-32 border-t-2 border-r-2 border-slate-200 rounded-tr-3xl" />
          </div>
        </div>

        {/* Right Side: Auth Forms */}
        <div className="w-full md:w-7/12 p-8 md:p-16 flex flex-col justify-center items-center relative overflow-y-auto bg-white/60">
          <div className="w-full max-w-lg py-8">
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
          </div>
        </div>
      </div>
    </div>
  );
};

export default AuthPage;
